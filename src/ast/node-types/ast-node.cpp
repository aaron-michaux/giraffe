#include "stdinc.hpp"

#include "ast-node.hpp"

namespace giraffe
{
const char* str(NodeType o) noexcept
{
#define CASE(x) \
   case NodeType::x: return #x
   switch(o) {
      CASE(NONE);
      CASE(TRANSLATION_UNIT);
      CASE(MODULE);
      CASE(IFTHEN);
      CASE(COMMAND);
      CASE(EXPRESSION);
   }
#undef CASE
   assert(false);
   return "<unknown>";
}

} // namespace giraffe
