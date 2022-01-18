
#include "stdinc.hpp"

#include "eval-expression.hpp"
#include "eval.hpp"
#include "symbol-table.hpp"

namespace giraffe
{

bool evaluate_condition(Context& context, SymbolTable& symbols, const IfThenNode* node);
void eval_ifthen_node(EvalContext& eval_ctx, const IfThenNode* node);
void eval_define_node(EvalContext& eval_ctx, const DefineNode* node);
void eval_undef_node(EvalContext& eval_ctx, const UndefNode* node);
void eval_error_node(EvalContext& eval_ctx, const ErrorNode* node);
void eval_module_node(EvalContext& eval_ctx, const ModuleNode* node);
void eval_include_node(EvalContext& eval_ctx, const IncludeNode* node);

// ------------------------------------------------------------------------------ Evaluate Condition

bool evaluate_condition(Context& context, SymbolTable& symbols, const IfThenNode* node)
{
   string_view symbol = node->symbol();
   switch(node->type()) {
   case IfThenType::NONE: context.push_error(node->loc0(), "logic error evaluating if-then"); break;
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
}

// ------------------------------------------------------------------------------------ eval if-then

void eval_ifthen_node(EvalContext& eval_ctx, const IfThenNode* node)
{
   auto& context = eval_ctx.current_context();

   // Must be IF, IFDEF, or IFNDEF
   if(!node->is_if()) {
      context.push_error(node->loc0(), "unexpcted preprocessor directive");
      return;
   }

   for(size_t i = 0; i < node->n_if_elif_else_parts(); ++i) {
      auto child = node->if_elif_part(i);
      if(evaluate_condition(context, eval_ctx.symbols(), child)) {
         eval_node(eval_ctx, child->stmts());
         break;
      }
   }
}

// ------------------------------------------------------------------------------------- eval define

void eval_define_node(EvalContext& eval_ctx, const DefineNode* node)
{
   auto& context = eval_ctx.current_context();
   auto& symbols = eval_ctx.symbols();
   if(symbols.has(node->identifier()))
      context.push_error(node->loc1(), format("symbol '{}' already defined", node->identifier()));
   else
      symbols.insert(node->identifier(), node->arglist(), node->text());
}

// -------------------------------------------------------------------------------------- eval undef

void eval_undef_node(EvalContext& eval_ctx, const UndefNode* node)
{
   auto& context = eval_ctx.current_context();
   auto& symbols = eval_ctx.symbols();
   if(symbols.has(node->identifier()))
      symbols.remove(node->identifier());
   else
      context.push_error(node->loc1(), format("symbol '{}' not defined", node->identifier()));
}

// -------------------------------------------------------------------------------------- eval error

void eval_error_node(EvalContext& eval_ctx, const ErrorNode* node)
{
   auto& context = eval_ctx.current_context();
   if(node->is_error())
      context.push_error(node->loc0(), node->message().data());
   else
      context.push_warn(node->loc0(), node->message().data());
}

// ------------------------------------------------------------------------------------- eval module

void eval_module_node(EvalContext& eval_ctx, const ModuleNode* node)
{
   // Note if file depends on a module
   // Note if file provides a module
}

// ------------------------------------------------------------------------------------ eval include

void eval_include_node(EvalContext& eval_ctx, const IncludeNode* node)
{
   // Find the file (using the include path)
   const auto path = eval_ctx.resolve_include_path(node->filename(), node->is_local_include());
   if(!path.is_found) {
      eval_ctx.current_context().push_error(node->loc0(), "could not resolve include");
      return;
   }

   // Note the dependency in the EvalContext
   eval_ctx.process_include(path.filename, path.is_isystem_path);
}

// --------------------------------------------------------------------------------------- eval stmt

void eval_node(EvalContext& eval_ctx, const AstNode* node) noexcept
{
   auto& context = eval_ctx.current_context();

   switch(node->node_type()) {
   case NodeType::NONE: [[fallthrough]]; // abort
   case NodeType::EMPTY:                 // error: should not be evaluating on a parse error
      context.push_error(node->loc0(), "logic error: evaluating an empty expression");
      break;
   case NodeType::STMT_LIST: [[fallthrough]];
   case NodeType::TRANSLATION_UNIT:
      for(auto child : *node) eval_node(eval_ctx, child);
      break;
   case NodeType::MODULE: eval_module_node(eval_ctx, cast_ast_node<ModuleNode>(node)); break;
   case NodeType::IFTHEN: eval_ifthen_node(eval_ctx, cast_ast_node<IfThenNode>(node)); break;
   case NodeType::DEFINE: eval_define_node(eval_ctx, cast_ast_node<DefineNode>(node)); break;
   case NodeType::UNDEF: eval_undef_node(eval_ctx, cast_ast_node<UndefNode>(node)); break;
   case NodeType::INCLUDE: eval_include_node(eval_ctx, cast_ast_node<IncludeNode>(node)); break;
   case NodeType::ERROR: eval_error_node(eval_ctx, cast_ast_node<ErrorNode>(node)); break;
   case NodeType::EXPRESSION:
      context.push_error(node->loc0(), "attempt to evaluate expression as statement");
      break;
   }
   assert(false);
}

} // namespace giraffe
