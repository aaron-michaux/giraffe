
#pragma once

namespace giraffe
{
struct DriverOptions final
{
   bool color_diagnostics                = true;
   bool w_error                          = false;
   vector<string> include_paths          = {};
   unordered_map<string, string> defines = {};
};

} // namespace giraffe
