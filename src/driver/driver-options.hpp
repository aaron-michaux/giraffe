
#pragma once

namespace giraffe
{
struct DriverOptions final
{
   bool color_diagnostics                                                          = true;
   bool w_error                                                                    = false;
   vector<sso23::string> include_paths                                             = {};
   unordered_map<sso23::string, sso23::string, sso23::hash<sso23::string>> defines = {};
};

} // namespace giraffe
