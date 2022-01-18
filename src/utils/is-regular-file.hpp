
#pragma once

#include <filesystem>
#include <string_view>

namespace giraffe
{
inline bool is_regular_file(std::string_view filename) noexcept
{
   try {
      auto p = std::filesystem::path(filename);
      return std::filesystem::is_regular_file(p);
   } catch(...) {}
   return false;
}
} // namespace giraffe
