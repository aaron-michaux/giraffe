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
   if(token.id() == TSTRING) {
      scanner.consume();
      assert(token.text().size() >= 2);
      auto filename = string{token.text().begin() + 1,
         token.text().begin() + token.text().size() - 1};      
      return new IncludeNode{token.source_range(), move(filename), false};
   } else if(token.id() == TSTR_DELIM) {
      auto [filename, src_range] = accept_cstr(context);
      return new IncludeNode{src_range, std::move(filename), true};
   } else if(token.id() == TSPACESHIP) {
      return new IncludeNode{token.source_range(), "=", false};
   }

   // -- ERROR
   return on_error(format("expected filename after #include"));
}
}

