#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
AstNode * accept_include(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   auto on_error = [&] (string&& message) {
      context.push_error(std::move(message));
      skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
      return make_empty_node();
   };
   
   // #include (c-string | <FILENAME>)
   expect(scanner, TINCLUDE);
   scanner.consume();
   const auto& token = scanner.current();
   if(token.id() == TSTRING) {
      scanner.consume();
      return new IncludeNode(token.text());
   } else if(token.id() == TSTR_DELIM) {
      return new IncludeNode(accept_cstr(context));
   }

   // -- ERROR
   return on_error(format("expected filename after #include"));
}
}

