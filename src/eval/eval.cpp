
#include "stdinc.hpp"

#include "eval.hpp"

#include "driver/symbol-table.hpp"

#include "eval-expression.hpp"

namespace giraffe
{

bool evaluate_condition(Context& context, SymbolTable& symbols, const IfThenNode* node);
void eval_ifthen_node(Context& context, const IfThenNode* node);
void eval_define_node(Context& context, const DefineNode* node);
void eval_undef_node(Context& context, const UndefNode* node);
void eval_error_node(Context& context, const ErrorNode* node);
void eval_module_node(Context& context, const ModuleNode* node);
void eval_include_node(Context& context, const IncludeNode* node);

// ------------------------------------------------------------------------------ Evaluate Condition

bool evaluate_condition(Context& context, SymbolTable& symbols, const IfThenNode* node)
{
   string_view symbol = node->symbol();
   switch(node->type()) {
   case IfThenType::NONE: context.push_error(node->loc0(), "logic error evaluating if-then"); break;
   case IfThenType::IF: [[fallthrough]];
   case IfThenType::ELIF: return evaluate_expr(context, node->condition()).is_true();
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
}

// ------------------------------------------------------------------------------------ eval if-then

void eval_ifthen_node(Context& context, const IfThenNode* node)
{
   // Must be IF, IFDEF, or IFNDEF
   if(!node->is_if()) {
      context.push_error(node->loc0(), "unexpcted preprocessor directive");
      return;
   }

   for(size_t i = 0; i < node->n_if_elif_else_parts(); ++i) {
      auto child = node->if_elif_part(i);
      if(evaluate_condition(context, context.symbols(), child)) {
         eval_node(context, child->stmts());
         break;
      }
   }
}

// ------------------------------------------------------------------------------------- eval define

void eval_define_node(Context& context, const DefineNode* node)
{
   auto& symbols = context.symbols();
   if(symbols.has(node->identifier()))
      context.push_error(node->loc1(), format("symbol '{}' already defined", node->identifier()));
   else
      symbols.insert(node->identifier(), node->arglist(), node->token_sequence());
}

// -------------------------------------------------------------------------------------- eval undef

void eval_undef_node(Context& context, const UndefNode* node)
{
   auto& symbols = context.symbols();
   if(symbols.has(node->identifier()))
      symbols.remove(node->identifier());
   else
      context.push_error(node->loc1(), format("symbol '{}' not defined", node->identifier()));
}

// -------------------------------------------------------------------------------------- eval error

void eval_error_node(Context& context, const ErrorNode* node)
{
   if(node->is_error())
      context.push_error(node->loc0(), node->message().data());
   else
      context.push_warn(node->loc0(), node->message().data());
}

// ------------------------------------------------------------------------------------- eval module

void eval_module_node(Context& context, const ModuleNode* node)
{
   // Note if file depends on a module
   // Note if file provides a module
}

// ------------------------------------------------------------------------------------ eval include
void eval_include_node(Context& context, const IncludeNode* node)
{
   // Find the file (using the include path)
   const auto path = context.resolve_include_path(node->filename(), node->is_local_include());
   if(!path.is_found) {
      context.push_error(node->loc0(), "could not resolve include");
      return;
   }

   // Note the dependency in the Context
   context.process_include(path.filename, path.is_isystem_path);
}

// --------------------------------------------------------------------------------------- eval stmt

void eval_node(Context& context, const AstNode* node) noexcept
{
   switch(node->node_type()) {
   case NodeType::NONE: [[fallthrough]]; // abort
   case NodeType::EMPTY:                 // error: should not be evaluating on a parse error
      context.push_error(node->loc0(), "logic error: evaluating an empty expression");
      break;
   case NodeType::STMT_LIST: [[fallthrough]];
   case NodeType::TRANSLATION_UNIT:
      for(auto child : *node) eval_node(context, child);
      break;
   case NodeType::MODULE: eval_module_node(context, cast_ast_node<ModuleNode>(node)); break;
   case NodeType::IFTHEN: eval_ifthen_node(context, cast_ast_node<IfThenNode>(node)); break;
   case NodeType::DEFINE: eval_define_node(context, cast_ast_node<DefineNode>(node)); break;
   case NodeType::UNDEF: eval_undef_node(context, cast_ast_node<UndefNode>(node)); break;
   case NodeType::INCLUDE: eval_include_node(context, cast_ast_node<IncludeNode>(node)); break;
   case NodeType::ERROR: eval_error_node(context, cast_ast_node<ErrorNode>(node)); break;
   case NodeType::EXPRESSION:
      context.push_error(node->loc0(), "attempt to evaluate expression as statement");
      break;
   }
}

} // namespace giraffe
