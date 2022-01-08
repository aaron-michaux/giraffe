#include "stdinc.hpp"

#include "ast/ast.hpp"
#include "if-then-node.hpp"

#define This IfThenNode

namespace giraffe
{

// ---------------------------------------------------------------- make-if-part

IfThenNode* This::make_if_part(IfThenType type, vector<AstNode*>&& children) noexcept
{
   auto node = unique_ptr<IfThenNode>(new IfThenNode(type));
   assert(node->is_if());
   assert(node->has_condition());
   assert(children.size() >= 2);
   assert(children[0]->node_type() == NodeType::EXPRESSION);
   assert(children[1]->node_type() == NodeType::STMT_LIST);
   for(size_t i = 2; i < children.size(); ++i) assert(children[i]->node_type() == NodeType::IFTHEN);
   node->set_children_(std::move(children));
   return node.release();
}

// -------------------------------------------------------------- make-elif-part

IfThenNode*
This::make_elif_part(IfThenType type, ExpressionNode* condition, StmtListNode* stmt_list) noexcept
{
   auto node = unique_ptr<IfThenNode>(new IfThenNode(type));
   assert(!node->is_if());
   node->set_children_(condition, stmt_list);
   return node.release();
}

// --------------------------------------------------------------------- getters

const ExpressionNode* This::condition() const noexcept
{
   return cast_ast_node<ExpressionNode>(children()[0]);
}

const StmtListNode* This::stmts() const noexcept
{
   return cast_ast_node<StmtListNode>(children()[1]);
}

bool This::is_if() const noexcept
{
   return type() == IfThenType::IF || type() == IfThenType::IFDEF || type() == IfThenType::IFNDEF;
}

bool This::has_condition() const noexcept
{
   return type() != IfThenType::ELSE && type() != IfThenType::NONE;
}

string_view This::symbol() const noexcept
{
   // IFDEF, IFNDEF, ELIFDEF, ELIFNDEF only
   if(children().size() >= 2 && condition()->expr_type() == ExprType::IDENTIFIER) {
      return condition()->text();
   }
   return "";
}

size_t This::n_if_elif_else_parts() const noexcept
{
   // #if, #elif, ..., #else #fi
   assert(children().size() >= 2);
   return is_if() ? (1 + children().size() - 2) : 0;
}

const IfThenNode* This::if_elif_part(size_t index) const noexcept
{
   assert(is_if());
   if(index == 0) { return this; }
   return cast_ast_node<IfThenNode>(children()[index + 2 - 1]);
}

// ---------------------------------------------------------------------- stream

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   // The indent
   for(auto i = 0; i < indent; ++i) ss << ' ';

   // Print the command
   switch(type()) {
   case IfThenType::NONE: ss << "#<IfThen::NONE>"; break;
   case IfThenType::IF: ss << "#if"; break;
   case IfThenType::IFDEF: ss << "#ifdef"; break;
   case IfThenType::IFNDEF: ss << "#ifndef"; break;
   case IfThenType::ELIF: ss << "#elif"; break;
   case IfThenType::ELIFDEF: ss << "#elifdef"; break;
   case IfThenType::ELIFNDEF: ss << "#elifndef"; break;
   case IfThenType::ELSE: ss << "#else"; break;
   }

   // Print the condition
   if(has_condition()) {
      ss << ' ';
      condition()->stream(ss, 0); // Should be on 1 line
   }
   ss << '\n';

   // Print the statement list
   for(const auto stmt : stmts()->children()) stmt->stream(ss, indent + 1);

   // Print the #elif, #elifdef, #elifndef, #else parts
   for(size_t i = 2; i < children().size(); ++i) children()[i]->stream(ss, indent);

   // Print #endif iff #if, #ifdef, #ifndef
   if(is_if()) {
      for(auto i = 0; i < indent; ++i) ss << ' ';
      ss << "#endif" << '\n';
   }

   return ss;
}

} // namespace giraffe
#undef This
