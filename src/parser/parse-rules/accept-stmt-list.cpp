#include "stdinc.hpp"

#include <deque>

#include "parser/parser-internal.hpp"

namespace giraffe
{
//
// StmtList:
//    | PreprocCommand*
//    | 'export'? ('import' | 'module') ModuleName ';'
//    ;
//
StmtListNode* accept_stmt_list(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   std::deque<AstNode*> children;
   auto push_child = [&](AstNode* child) {
      assert(child != nullptr);
      children.push_back(child);
   };

   while(!scanner.current().is_eof()) {
      const auto id = scanner.current().id();
      if(id == TNEWLINE) {
         scanner.consume();

      } else if(in_list(id, first_set_module)) {
         push_child(accept_module(context));

      } else if(id == TDEFINE) {
         push_child(accept_define(context));

      } else if(id == TINCLUDE) {
         push_child(accept_include(context));

      } else if(id == TUNDEF) {
         push_child(accept_undef(context));

      } else if(in_list(id, first_set_error_warning)) {
         push_child(accept_error(context));

      } else if(in_list(id, first_set_ifthen)) {
         push_child(accept_if_then(context));

      } else if(in_list(id, stray_ifthen_parts)) {
         break; // we're done

      } else {
         // Assume this is just the blah-blah-blah of the document
         skip_to_sequence(scanner, TNEWLINE); // Skip to newline
      }
   }

   //
   return new StmtListNode({begin(children), end(children)});
}

} // namespace giraffe
