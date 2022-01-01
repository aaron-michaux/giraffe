#include "stdinc.hpp"

#include "error-node.hpp"

#define This ErrorNode


namespace giraffe {

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   for(auto i = 0; i < indent; ++i) ss << ' ';
   ss << (is_error() ? "#error" : "#warning") << message() << '\n';
   return ss;
}

}
#undef This
