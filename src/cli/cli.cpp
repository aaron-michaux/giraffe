#include "stdinc.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>

#include <unistd.h>
#include <libgen.h>

#include "cli.hpp"

#include "cli-args.hpp"

#include "driver/context.hpp"
#include "driver/execute.hpp"

namespace giraffe
{
static string read_stream(auto&& in)
{
   in >> std::noskipws;
   std::istream_iterator<char> bb(in);
   std::istream_iterator<char> ee;
   return std::string(bb, ee);
}

static bool is_regular_file(string_view filename) noexcept
{
   try {
      auto p = std::filesystem::path(filename);
      return std::filesystem::is_regular_file(p);
   } catch(...) {}
   return false;
}

static void add_define(auto& map, string_view arg) noexcept
{
   // symbol=value
   const auto pos = arg.find('=');
   const auto has_eq = (pos != string_view::npos);
   string key = string(begin(arg), has_eq ? begin(arg) + pos : end(arg));
   string val = has_eq ? string(begin(arg) + pos + 1, end(arg)) : ""s;
   map.insert_or_assign(std::move(key), std::move(val));
}

// ------------------------------------------------------------------ operator<<

void print_range(ostream& os, auto delim, auto&& input_range, auto f) {
   bool is_first = true;
   for(const auto& value: input_range) {
      if(!is_first) os << delim;
      is_first = false;
      os << f(value);
   }
}

ostream& operator<<(ostream& os, const CliConfig& conf)
{   
   os << format(R"V0G0N(
show-help:      {}
has-error:      {}
dump-tokens:    {}
filename:       {}
compiler-opts
   color-diag:  {}
   w-error:     {}
   inc-dirs:    {}
   defines:     {}
)V0G0N",
                conf.show_help,
                conf.has_error,
                conf.dump_tokens,
                conf.filename,
                conf.driver_opts.color_diagnostics,
                conf.driver_opts.w_error,
                range::implode(conf.driver_opts.include_paths, ", "),
                range::implode(conf.driver_opts.defines, ", ", [] (auto& x) {
                      if(x.second.empty()) return x.first;
                      return format("{}={}", x.first, x.second);
                   }));
   
   // print_range(os, "', '", conf.include_paths, [] (auto& x) { return x; });
   // os << "\ndefines:   '";   
   // print_range(os, ", ", conf.defines, [] (auto& x) {
   //       if(x.second.empty()) return x.first;
   //       return format("{}={}", x.first, x.second);
   //    });
   // os << '\n';
   return os;
}

// ------------------------------------------------------------------- show-help

void show_help(char* argv0) noexcept
{
   cout << format(R"V0G0N(

   Usage: {} [OPTIONS...] <filename>

   Options:
  
      -Dsymbol(=value)            Define a symbol
      -Ipath                      Add an include path
      -isystempath                Add an include path

      -fno-color-diagnostics      Turn off color diagnostics.
      -fcolor-diagnostics-always  Color nven no terminal is attached.

)V0G0N",
                  basename(argv0));
}

// ---------------------------------------------------------- parse-command-line

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
         else if(arg == "--dump-tokens")
            config.dump_tokens = true;
         else if(arg.starts_with("-D"))
            add_define(config.driver_opts.defines,
                       string_view(begin(arg)+2, end(arg)));
         else if(arg.starts_with("-I"))
            config.driver_opts.include_paths.emplace_back(begin(arg)+2, end(arg));
         else if(arg.starts_with("-isystem"))
            config.driver_opts.include_paths.emplace_back(begin(arg)+8, end(arg));
         else if(is_regular_file(arg) && config.filename.empty())
            config.filename = string(begin(arg), end(arg));
         else if(arg == "-" && config.filename.empty()) // i.e., stdin
            config.filename = "-"s;
         else {
            std::cerr << format("Unexpected command-line argument: '{}'\n",
                                arg);
            config.has_error = true;
         }
      } catch(std::exception& e) {
         std::cerr << format(
             "Exception at command-line argument '{}': {}\n", arg, e.what());
         config.has_error = true;
      }
   }

   // Handle color diagnostics
   config.driver_opts.color_diagnostics = (isatty(1) != 0);
   if(no_color_diagnostics) config.driver_opts.color_diagnostics = false;
   if(color_diagnostics_always) config.driver_opts.color_diagnostics = true;

   if(config.filename.empty()) {
      std::cerr << format("Must specify at an input file!\n");
      config.has_error = true;
   }

   return config;
}

// ---------------------------------------------------------------- init context

static unique_ptr<Context> init_compiler_context(const CliConfig& config) noexcept
{
   if(config.has_error) return nullptr;
   
   try {
      auto source = (config.filename == "-")
         ? make_unique<FILE_ScannerInput>("<stdin>", stdin)
         : make_unique<FILE_ScannerInput>(config.filename);
      auto scanner = make_unique<giraffe::Scanner>(std::move(source));
      return Context::make(std::move(scanner), config.driver_opts);
   } catch(std::exception& e) {
      std::cerr << format("Exception while reading '{}': {}",
                          config.filename,
                          e.what())
                << endl;
   }
   return nullptr; // error
}

// ------------------------------------------------------------------------- run

int run(int argc, char** argv) noexcept
{
   const auto config = parse_command_line(argc, argv);
   if(config.show_help) {
      show_help(argv[0]);
      return EXIT_SUCCESS;
   }
   
   auto context = init_compiler_context(config);
   if(context == nullptr) {
      cout << "Aborting due to previous errors." << endl;
      return EXIT_FAILURE;
   }
   
   const auto success = execute(*context);
   return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace giraffe
