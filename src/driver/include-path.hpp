
#pragma once

#include <string>

namespace giraffe
{
struct IncludePath
{
   std::string path = {};
   bool is_isystem  = false;
};

struct ResolvedPath
{
   std::string filename = {};
   bool is_found        = false;
   bool is_isystem_path = false;
};

ResolvedPath resolve_include_path(const vector<IncludePath>& include_paths,
                                  string_view filename,
                                  bool is_local_include) noexcept;

} // namespace giraffe
