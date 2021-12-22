#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
IfThenNode * accept_if_then(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   expect(scanner, first_set_ifthen);
   
   
   
   //return IfThenNode::make({ begin(children), end(children) });
   return nullptr;
}

}

