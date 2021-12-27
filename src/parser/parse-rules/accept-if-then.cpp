#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

IfThenType token_id_to_if_then_type(int id)
{
   switch(id) {
   case TIF: return IfThenType::IF;
   case TIFDEF: return IfThenType::IFDEF;
   case TIFNDEF: return IfThenType::IFNDEF;
   case TELIF: return IfThenType::ELIF;
   case TELIFDEF: return IfThenType::ELIFDEF;
   case TELIFNDEF: return IfThenType::ELIFNDEF;
   case TELSE: return IfThenType::ELSE;
   default:
      FATAL("logic error");
   }
   return IfThenType::NONE;
}

AstNode * accept_else_elif_part(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   expect(scanner, first_set_else_elif_parts);

   // The type: #else | #elif | #elifdef | #elifndef
   const auto type = token_id_to_if_then_type(scanner.consume().id());   
   const bool is_else = (type == IfThenType::ELSE);

   // Accept the condition
   unique_ptr<ExpressionNode> condition = {};
   if(is_else) {
      condition.reset(ExpressionNode::make_empty());
   } else {
      condition.reset(accept_expression(context));
   }
   assert(condition != nullptr);

   // Ensure no stray tokens after condition
   if(scanner.current().id() != TNEWLINE) {
      context.push_error("unexpected token!");
      skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
   }   

   // Accept the statment list
   unique_ptr<StmtListNode> stmts{accept_stmt_list(context)};
   assert(stmts != nullptr);
   
   return IfThenNode::make_elif_part(type, condition.release(), stmts.release());
}



AstNode * accept_if_then(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   expect(scanner, first_set_ifthen);

   vector<AstNode*> children;
   
   const auto type = token_id_to_if_then_type(scanner.consume().id());   
   children.push_back(accept_expression(context));
   assert(children.back() != nullptr);

   // Ensure no stray tokens after condition
   if(scanner.current().id() != TNEWLINE) {
      context.push_error("unexpected token!");
      skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
   }   

   // Accept the statment list
   children.push_back(accept_stmt_list(context));
   assert(children.back() != nullptr);

   // Get the elif parts
   while(in_list(scanner.current().id(), first_set_else_elif_parts)) {
      children.push_back(accept_else_elif_part(context));
      assert(children.back() != nullptr);
   }
   
   // Get the endif part
   if(scanner.current().id() != TENDIF) {
      context.push_error("unexpected token!");
      skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
   } else {
      scanner.consume();
   }

   return IfThenNode::make_if_part(type, std::move(children));
}

}

