
#pragma once

#include "driver/include-path.hpp"
#include "driver/symbol-table.hpp"

namespace giraffe
{
struct ConfigFileData
{
   using string_pair_type            = std::pair<sso23::string, sso23::string>;
   vector<IncludePath> include_paths = {};
   vector<string_pair_type> defines  = {};

   static ConfigFileData read(string_view filename) noexcept(false);
};

} // namespace giraffe
