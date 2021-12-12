
#pragma once

#include "driver/driver-options.hpp"

namespace giraffe
{
struct CliConfig final
{
   bool show_help                        = false;
   bool has_error                        = false;   
   string filename                       = ""s;
   vector<string> include_paths          = {};
   unordered_map<string, string> defines = {};
   DriverOptions driver_opts             = {};

   friend ostream& operator<<(const CliConfig&, ostream&);
};

CliConfig parse_command_line(int argc, char** argv) noexcept;
void show_help(char* argv0) noexcept;

int run(int argc, char** argv) noexcept;

} // namespace giraffe
