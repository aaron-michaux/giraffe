#include "stdinc.hpp"

#include "include-node.hpp"

#define This IncludeNode


namespace giraffe {

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   for(auto i = 0; i < indent; ++i) ss << ' ';
   ss << "#include ";
   ss << (is_local_include() ? '"' : '<');
   encode_string(ss, filename());
   ss << (is_local_include() ? '"' : '>');
   ss << '\n';
   return ss;
}

}
