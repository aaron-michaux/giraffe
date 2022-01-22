
#include "stdinc.hpp"

#include "include-path.hpp"

namespace giraffe
{

ResolvedPath resolve_include_path(const vector<IncludePath>& include_paths,
                                  string_view filename,
                                  bool is_local_include) noexcept
{
   ResolvedPath ret = {};

   auto test_it = [&filename, &ret](string_view dir, bool is_isystem) {
      std::string path = format("{}{}{}", dir, (dir.empty() ? "" : "/"), filename);
      if(std::filesystem::exists(path)) {
         ret.filename        = std::move(path);
         ret.is_found        = true;
         ret.is_isystem_path = is_isystem;
      }
   };

   // Test for local include if `is_local_include`
   if(is_local_include) test_it("", false);

   // Test other paths
   for(auto ii = begin(include_paths); !ret.is_found && ii != end(include_paths); ++ii)
      test_it(ii->path, ii->is_isystem);

   return ret;
}

} // namespace giraffe
