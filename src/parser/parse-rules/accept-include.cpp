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
   assert(expect(scanner, TINCLUDE));
   scanner.consume();
   const auto& token = scanner.current();
   if(token.id() == TSTRING || token.id() == TSPACESHIP) {
      scanner.consume();
      return new IncludeNode(token.source_range(), token.text());
   } else if(token.id() == TSTR_DELIM) {
      auto str = accept_cstr(context);
      return new IncludeNode{str.second, str.first};
   }

   // -- ERROR
   return on_error(format("expected filename after #include"));
}
}

