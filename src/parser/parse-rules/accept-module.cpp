#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
// 'module' ';'
// 'module' ModuleName ';'
// 'export' 'module' ModuleName ';'
// 'export' 'import'? ModuleName ';'
// 'import' 'export'? ModuleName ';'
// 'import' <include> ';'
// 'import' "include" ';'
AstNode* accept_module(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_module));

   auto on_error = [&](std::string&& message) {
      context.push_error(std::move(message));
      skip_to_sequence(scanner, TNEWLINE); // Skip to newline
      return make_empty_node();
   };

   SourceRange loc        = {scanner.current().location(), {}};
   bool is_import         = false;
   bool is_export         = false;
   bool is_module         = false;
   bool is_local_include  = false; // Something like: import "something.hpp";
   bool is_system_include = false; // Something like: import <iostream>;
   sso23::string text     = {};

   while(scanner.current().id() == TEXPORT || scanner.current().id() == TIMPORT) {
      auto id   = scanner.consume().id();
      is_export = is_export || (id == TEXPORT);
      is_import = is_import || (id == TIMPORT);
   }

   if(scanner.current().id() == TMODULE) {
      if(is_import) return on_error("unexpected 'module' keyword after 'import'");
      scanner.consume();
      is_module = true;
   }

   if(!is_export && !is_import && !is_module) {
      assert(false); // logic error
   }

   // We require an IDENTIFIER... the name of the module being
   // exported (or export imported)
   if(scanner.current().id() == TIDENTIFIER) {
      text = scanner.consume().text();
   } else if(scanner.current().id() == TSTRING) {
      is_system_include = true;
      auto sv           = scanner.consume().text();
      assert(sv.size() >= 2);
      text = sso23::string{sv.begin() + 1, sv.begin() + sv.size() - 1};
   } else if(scanner.current().id() == TSTR_DELIM) {
      is_local_include           = true;
      auto [filename, src_range] = accept_cstr(context);
      text                       = std::move(filename);
   } else if(is_import || is_export) {
      return on_error("expected module name");
   }

   // Consume the semicolon
   if(scanner.current().id() != TSEMICOLON) {
      return on_error("expected semicolon after module delcaration");
   } else {
      loc.second = scanner.consume().location();
   }

   return new ModuleNode(
       loc, is_import, is_export, is_module, is_local_include, is_system_include, text);
}
} // namespace giraffe
