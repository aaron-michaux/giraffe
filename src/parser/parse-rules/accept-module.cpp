#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
//  'export'? ('import' | 'module') ModuleName ';'
AstNode * accept_module(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   expect(scanner, first_set_module);


   
   return new EmptyNode();
}
}

