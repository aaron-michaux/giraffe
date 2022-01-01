#include "stdinc.hpp"

#include "undef-node.hpp"

#define This UndefNode

namespace giraffe
{

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   for(auto i = 0; i < indent; ++i) ss << ' ';
   ss << "#undef " << identifier() << '\n';
   return ss;
}

} // namespace giraffe
#undef This
