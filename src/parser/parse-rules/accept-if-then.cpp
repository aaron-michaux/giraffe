#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

AstNode * accept_else_elif_part(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   expect(scanner, first_set_else_elif_parts);

   // #else
   // (#elif | #elifdef | #elifndef) CONDITION
   // stmt-list
   return nullptr;
}



AstNode * accept_if_then(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   expect(scanner, first_set_ifthen);
   
   

   
   //return IfThenNode::make({ begin(children), end(children) });
   return nullptr;
}

}

