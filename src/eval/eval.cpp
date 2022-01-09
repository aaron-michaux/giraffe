
#include "stdinc.hpp"

#include "eval-expression.hpp"
#include "eval.hpp"
#include "symbol-table.hpp"

namespace giraffe
{

void eval_stmt(Context& context, SymbolTable& symbols, const AstNode* node) noexcept
{
   auto eval_module_node = [&context, &symbols](const ModuleNode* node) {
      // Note if file depends on a module
      // Note if file provides a module
   };

   auto evaluate_condition = [](Context& context, SymbolTable& symbols, const IfThenNode* node) {
      string_view symbol = node->symbol();
      switch(node->type()) {
      case IfThenType::NONE:
         context.push_error(node->loc0(), "logic error evaluating if-then");
         break;
      case IfThenType::IF: [[fallthrough]];
      case IfThenType::ELIF: return evaluate_expr(context, symbols, node->condition()).is_true();
      case IfThenType::IFDEF: [[fallthrough]];
      case IfThenType::ELIFDEF:
         if(symbol.empty()) {
            context.push_error(node->loc0(), "expected symbol after #ifdef/#elifdef");
            return false;
         } else {
            return symbols.has(symbol);
         }
      case IfThenType::IFNDEF: [[fallthrough]];
      case IfThenType::ELIFNDEF:
         if(symbol.empty()) {
            context.push_error(node->loc0(), "expected symbol after #ifndef/#elifndef");
            return false;
         } else {
            return !symbols.has(symbol);
         }
      case IfThenType::ELSE: return true; // Always execute an else statment!
      }
      assert(false); // logic error
      return false;
   };

   auto eval_ifthen_node = [&](const IfThenNode* node) {
      // Must be IF, IFDEF, or IFNDEF
      if(!node->is_if()) {
         context.push_error(node->loc0(), "unexpcted preprocessor directive");
         return;
      }

      for(size_t i = 0; i < node->n_if_elif_else_parts(); ++i) {
         auto child = node->if_elif_part(i);
         if(evaluate_condition(context, symbols, child)) {
            eval_stmt(context, symbols, child->stmts());
            break;
         }
      }
   };

   auto eval_define_node = [&context, &symbols](const DefineNode* node) {
      if(symbols.has(node->identifier()))
         context.push_error(node->loc1(),
                            format("symbol '{}' already defined", node->identifier()));
      else
         symbols.insert(node->identifier(), node->arglist(), node->text());
   };

   auto eval_undef_node = [&context, &symbols](const UndefNode* node) {
      if(symbols.has(node->identifier()))
         symbols.remove(node->identifier());
      else
         context.push_error(node->loc1(), format("symbol '{}' not defined", node->identifier()));
   };

   auto eval_include_node = [&context, &symbols](const IncludeNode* node) {
      // Find the file (using the include path)
      // Note the dependency in the EvalContext
      // Boot up a new context, and parse-evaluate
   };

   auto eval_error_node = [&context, &symbols](const ErrorNode* node) {
      if(node->is_error())
         context.push_error(node->loc0(), node->message().data());
      else
         context.push_warn(node->loc0(), node->message().data());
   };

   switch(node->node_type()) {
   case NodeType::NONE: [[fallthrough]]; // abort
   case NodeType::EMPTY:                 // error: should not be evaluating on a parse error
      context.push_error(node->loc0(), "logic error: evaluating an empty expression");
      break;
   case NodeType::STMT_LIST: [[fallthrough]];
   case NodeType::TRANSLATION_UNIT:
      for(auto child : *node) eval_stmt(context, symbols, child);
      break;
   case NodeType::MODULE: eval_module_node(cast_ast_node<ModuleNode>(node)); break;
   case NodeType::IFTHEN: eval_ifthen_node(cast_ast_node<IfThenNode>(node)); break;
   case NodeType::DEFINE: eval_define_node(cast_ast_node<DefineNode>(node)); break;
   case NodeType::UNDEF: eval_undef_node(cast_ast_node<UndefNode>(node)); break;
   case NodeType::INCLUDE: eval_include_node(cast_ast_node<IncludeNode>(node)); break;
   case NodeType::ERROR: eval_error_node(cast_ast_node<ErrorNode>(node)); break;
   case NodeType::EXPRESSION:
      context.push_error(node->loc0(), "attempt to evaluate expression as statement");
      break;
   }
   assert(false);
}

void eval_stmts(Context& context, SymbolTable& symbols, const StmtListNode* stmt_list) noexcept {}

} // namespace giraffe
