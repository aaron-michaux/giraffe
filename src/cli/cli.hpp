
#pragma once

#include "driver/driver-options.hpp"

namespace giraffe
{
struct CliConfig final
{
   bool show_help = false;
   bool has_error = false;

   bool dump_tokens          = false;
   std::string filename      = ""s;
   DriverOptions driver_opts = {};

   friend ostream& operator<<(ostream&, const CliConfig&);
};

CliConfig parse_command_line(int argc, char** argv) noexcept;
void show_help(char* argv0) noexcept;

int run(int argc, char** argv) noexcept;

} // namespace giraffe
