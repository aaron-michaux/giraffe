#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
AstNode * accept_error(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   // #error|#warning <tokens>
   assert(expect(scanner, first_set_error_warning));
   const bool is_error = scanner.consume().id() == TERROR; // or TWARNING
   return new ErrorNode{is_error, accept_to_newline(context)};
}
}

