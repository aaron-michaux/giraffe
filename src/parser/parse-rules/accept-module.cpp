#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
//  'export'? ('import' | 'module') ModuleName ';'
AstNode * accept_module(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_module));

   auto on_error = [&] (string&& message) {
      context.push_error(std::move(message));
      skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
      return make_empty_node();
   };

   bool is_import = false;
   bool is_export = false;
   bool is_module = false;
   string identifier = {};
   
   if(scanner.current().id() == TEXPORT) {
      scanner.consume();
      is_export = true;
   }

   if(scanner.current().id() == TIMPORT) {
      scanner.consume();
      is_import = true;
   } else if(scanner.current().id() == TMODULE) {
      scanner.consume();
      is_module = true;
   } else {
      return on_error("expected 'import' or 'module' keyword");
   }

   if(scanner.current().id() == TIDENTIFIER) {
      const auto text = scanner.consume().text();
      identifier = string{text.begin(), text.end()};
   } else {
      return on_error("expected module name");
   }

   if(scanner.current().id() != TSEMICOLON) {
      return on_error("expected semicolon after module delcaration");      
   }
   
   return new ModuleNode(is_import, is_export, is_module, std::move(identifier));
}
}

