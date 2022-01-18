
#pragma once

#include <string>

namespace giraffe
{
struct IncludePath
{
   std::string path = ""s;
   bool is_isystem  = false;
};
} // namespace giraffe
