#include "stdinc.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>

#include <libgen.h>
#include <unistd.h>

#include "cli.hpp"

#include "cli-args.hpp"

#include "driver/context.hpp"

namespace giraffe
{
static CliConfig::string_pair_type make_define(string_view arg) noexcept
{
   // -Dsymbol=value => {symbol, value}
   const auto pos    = arg.find('=');
   const auto has_eq = (pos != string_view::npos);
   sso23::string key = sso23::string(begin(arg) + 2, has_eq ? begin(arg) + pos : end(arg));
   sso23::string val = has_eq ? sso23::string(begin(arg) + pos + 1, end(arg)) : "";
   return {std::move(key), std::move(val)};
}

static IncludePath make_include_path(string_view arg) noexcept
{
   const auto is_isystem = arg.starts_with("-isystem");
   const auto start      = begin(arg) + (is_isystem ? 8 : 2);
   return {std::string(start, end(arg)), is_isystem};
}

// -------------------------------------------------------------------------------------- operator<<

ostream& operator<<(ostream& os, const CliConfig& conf)
{
   auto format_path = [](const IncludePath& x) {
      return format("{}{}", (x.is_isystem ? "-isystem" : "-I"), x.path);
   };
   auto format_define = [](const auto& x) {
      if(x.second.empty()) { return format("-D{}", x.first); }
      return format("-D{}={}", x.first, x.second);
   };

   os << format(R"V0G0N(
show-help:         {}
has-error:         {}
dump-tokens:       {}
print-config:      {}
input-filename:    {}
output-filename:   {}
sys-config:        {}
compiler-opts
   color-diag:     {}
   w-error:        {}
   suppress-warn:  {}
   skip-system:    {}
includes:
   {}
defines:
   {}
)V0G0N",
                conf.show_help,
                conf.has_error,
                conf.dump_tokens,
                conf.print_config_and_exit,
                conf.input_filename,
                conf.output_filename,
                conf.config_filename,
                conf.driver_opts.color_diagnostics,
                conf.driver_opts.w_error,
                conf.driver_opts.suppress_warnings,
                conf.driver_opts.skip_system_includes,
                range::implode(conf.include_paths, "\n   ", format_path),
                range::implode(conf.defines, "\n   ", format_define));
   return os;
}

// --------------------------------------------------------------------------------------- show-help

void show_help(char* argv0) noexcept
{
   cout << format(R"V0G0N(

   Usage: {} [OPTIONS...] <filename>

   Options:

      -o <filename>               Output file. (stdout by default.)

      --sys-config <filename>     Config file produced by gcc, showing defines 
                                  and includes. See README.md.

      -Dsymbol(=value)?           Define a symbol.
      -Ipath                      Add an include path.
      -isystempath                Add an include path.

      -fno-color-diagnostics      Turn off color diagnostics.
      -fcolor-diagnostics-always  Color nven no terminal is attached.
      -fno-warn                   Suppress warnings.
      -Werror                     Warnings are errors.
      -fskip-system-includes      Do not process "isystem" includes.

)V0G0N",
                  basename(argv0));
}

// ------------------------------------------------------------------------------ parse-command-line

CliConfig parse_command_line(int argc, char** argv) noexcept
{
   CliConfig config = {};

   // First look for the help switch...
   for(auto i = 1; i < argc; ++i) {
      string_view arg = argv[i];
      if(arg == "-h" || arg == "--help") {
         config.show_help = true;
         return config;
      }
   }

   // Parse arguments
   bool no_color_diagnostics     = false;
   bool color_diagnostics_always = false;
   for(auto i = 1; i < argc; ++i) {
      string_view arg = argv[i];
      try {
         if(arg == "-fno-color-diagnostics")
            no_color_diagnostics = true;
         else if(arg == "-fcolor-diagnostics-always")
            color_diagnostics_always = true;
         else if(arg == "-fno-warn")
            config.driver_opts.suppress_warnings = true;
         else if(arg == "-fskip-system-includes")
            config.driver_opts.skip_system_includes = true;
         else if(arg == "-Werror")
            config.driver_opts.w_error = true;
         else if(arg == "--dump-tokens")
            config.dump_tokens = true;
         else if(arg == "-p")
            config.print_config_and_exit = true;
         else if(arg.starts_with("-D"))
            config.defines.emplace_back(make_define(arg));
         else if(arg.starts_with("-I") || arg.starts_with("-isystem"))
            config.include_paths.emplace_back(make_include_path(arg));
         else if(arg == "-o" || arg == "--output")
            config.output_filename = cli::safe_arg_str(argc, argv, i);
         else if(arg == "--sys-config")
            config.config_filename = cli::safe_arg_str(argc, argv, i);
         else if(is_regular_file(arg) && config.input_filename.empty())
            config.input_filename = std::string(begin(arg), end(arg));
         else if(arg == "-" && config.input_filename.empty()) // i.e., stdin
            config.input_filename = "-"s;
         else {
            std::cerr << format("Unexpected command-line argument: '{}'\n", arg);
            config.has_error = true;
         }
      } catch(std::exception& e) {
         std::cerr << format("Exception at command-line argument '{}': {}\n", arg, e.what());
         config.has_error = true;
      }
   }

   // Handle color diagnostics
   config.driver_opts.color_diagnostics = (isatty(1) != 0);
   if(no_color_diagnostics) config.driver_opts.color_diagnostics = false;
   if(color_diagnostics_always) config.driver_opts.color_diagnostics = true;

   if(config.input_filename.empty()) {
      std::cerr << format("Must specify at an input file!\n");
      config.has_error = true;
   }

   if(!config.config_filename.empty() && !is_regular_file(config.config_filename)) {
      std::cerr << format("File not found: '{}'\n", config.config_filename);
      config.has_error = true;
   }

   if(config.output_filename.starts_with("-")) {
      std::cerr << format("Cowardly refusing to write to suspicious looking output file: {}!\n",
                          config.output_filename);
      config.has_error = true;
   }

   return config;
}

// ------------------------------------------------------------------------------------ init context

static unique_ptr<ScannerInputInterface> init_input_source(const CliConfig& config) noexcept
{
   using namespace giraffe;

   if(config.has_error) return nullptr;

   try {
      return (config.input_filename == "-") ? make_unique<FILE_ScannerInput>("<stdin>", stdin)
                                            : make_unique<FILE_ScannerInput>(config.input_filename);
   } catch(std::exception& e) {
      std::cerr << format("Exception while reading '{}': {}", config.input_filename, e.what())
                << endl;
   }
   return nullptr; // error
}

// --------------------------------------------------------------------------------------------- run

bool run_config(const auto& config) noexcept
{
   bool has_error = false;

   // -- Create the input stream
   auto input_source = init_input_source(config);
   if(input_source == nullptr) has_error = true;

   // -- Get the output stream
   std::ofstream of; // RAII cleanup
   if(!config.output_filename.empty()) {
      of.open(config.output_filename);
      if(!of.is_open()) {
         std::cerr << format("Failed to open '{}' for writing", config.output_filename) << endl;
         has_error = true;
      }
   }
   std::ostream& ostream = config.output_filename.empty() ? std::cout : of;

   // -- Load the system config file
   ConfigFileData sys_config = {};
   try {
      sys_config = ConfigFileData::read(config.config_filename);
   } catch(std::exception& e) {
      std::cerr << format("Error reading config file '{}': {}\n", config.config_filename, e.what());
      has_error = true;
   }

   // -- Create the initial symbol table
   SymbolTable initial_symbol_table;
   FATAL(format("we have to fix this up with parsing!"));
   // for(const auto& [key, value] : sys_config.defines) initial_symbol_table.insert(key, {},
   // value); for(const auto& [key, value] : config.defines) initial_symbol_table.insert(key, {},
   // value);

   // -- Join the include paths together
   vector<IncludePath> include_paths = config.include_paths;
   include_paths.insert(
       end(include_paths), cbegin(sys_config.include_paths), cend(sys_config.include_paths));

   for(const auto& ipath : include_paths) {
      cout << format("{:10s} '{}'", ipath.is_isystem, ipath.path) << endl;
   }

   // -- Final error check
   if(has_error) {
      cout << "Aborting due to previous errors." << endl;
      return EXIT_FAILURE;
   }

   // -- Evaluate the input source
   auto scanner = make_unique<Scanner>(std::move(input_source));
   auto ctx     = Context::make(
       std::move(scanner), include_paths, std::move(initial_symbol_table), config.driver_opts);

   // -- Produce output
   if(!ctx->has_error()) { ctx->stream_make_rules(ostream); }

   const auto success = !ctx->has_error();
   return success;
}

// --------------------------------------------------------------------------------------------- run

int run(int argc, char** argv) noexcept
{
   const auto config = parse_command_line(argc, argv);
   if(config.show_help) {
      show_help(argv[0]);
      return EXIT_SUCCESS;
   } else if(config.print_config_and_exit) {
      cout << config << endl;
      return EXIT_SUCCESS;
   } else if(config.has_error) {
      cout << "Aborting due to previous errors." << endl;
      return EXIT_FAILURE;
   }

   const bool success = run_config(config);
   return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace giraffe
