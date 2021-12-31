#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
AstNode * accept_undef(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   // #undef IDENTIFIER
   assert(expect(scanner, TUNDEF));
   scanner.consume();
   const auto& token = scanner.current();
   if(token.id() == TIDENTIFIER) {
      scanner.consume();
      return new UndefNode(token.text());
   }

   // -- ERROR
   context.push_error(format("expected identifier after #undef"));
   skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
   return make_empty_node();
}
}

