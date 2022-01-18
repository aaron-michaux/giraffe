
#pragma once

#include "driver/driver-options.hpp"

namespace giraffe
{
struct CliConfig final
{
   using string_pair_type = std::pair<sso23::string, sso23::string>;

   bool show_help = false;
   bool has_error = false;

   bool dump_tokens              = false;
   sso23::string input_filename  = ""s;
   sso23::string output_filename = ""s;

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
