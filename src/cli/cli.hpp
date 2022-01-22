
#pragma once

#include "driver/driver-options.hpp"
#include "driver/include-path.hpp"

#include "read-config-file.hpp"

namespace giraffe
{
struct CliConfig final
{
   using string_pair_type = ConfigFileData::string_pair_type;
   bool show_help         = false;
   bool has_error         = false;

   bool dump_tokens            = false;
   bool print_config_and_exit  = false;
   std::string input_filename  = ""s;
   std::string output_filename = ""s;
   std::string config_filename = ""s;

   // {{ "-I", "/path/to/somewhere" }, { "-isystem", "/path/to/somewhere" }, ...
   vector<IncludePath> include_paths = {};
   vector<string_pair_type> defines  = {};

   DriverOptions driver_opts = {};

   friend ostream& operator<<(ostream&, const CliConfig&);
};

CliConfig parse_command_line(int argc, char** argv) noexcept;
void show_help(char* argv0) noexcept;

int run(int argc, char** argv) noexcept;

} // namespace giraffe
