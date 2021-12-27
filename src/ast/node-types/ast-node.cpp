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
      CASE(EMPTY);
      CASE(STMT_LIST);
      CASE(TRANSLATION_UNIT);
      CASE(MODULE);
      CASE(IFTHEN);
      CASE(DEFINE);
      CASE(UNDEF);
      CASE(INCLUDE);
      CASE(ERROR);
      CASE(EXPRESSION);
   }
#undef CASE
   assert(false);
   return "<unknown>";
}

} // namespace giraffe
