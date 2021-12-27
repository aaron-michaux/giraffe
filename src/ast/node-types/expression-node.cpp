#include "stdinc.hpp"

#include "expression-node.hpp"

#define This ExpressionNode


namespace giraffe {

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   ss << "TODO: expressions" << '\n';
   return ss;
}

}
#undef This

