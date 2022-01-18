
#pragma once

#include <string>

namespace giraffe
{
struct IncludePath
{
   std::string path = {};
   bool is_isystem  = false;
};
} // namespace giraffe
