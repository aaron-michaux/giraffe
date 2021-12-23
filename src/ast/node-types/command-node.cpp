#include "stdinc.hpp"

#include "command-node.hpp"

#define This CommandNode

namespace giraffe {

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{

   return ss;
}

}
