#include "stdinc.hpp"

#include "ast/ast.hpp"
#include "if-then-node.hpp"

#define This IfThenNode

namespace giraffe {

// ---------------------------------------------------------------- make-if-part

IfThenNode * This::make_if_part(IfThenType type,
                                vector<AstNode *>&& children) noexcept
{
   auto node = unique_ptr<IfThenNode>(new IfThenNode(type));
   assert(node->is_if());
   assert(node->has_condition());
   assert(children.size() >= 2);
   assert(children[0]->node_type() == NodeType::EXPRESSION);
   assert(children[1]->node_type() == NodeType::STMT_LIST);
   node->set_children(std::move(children));
   return node.release();
}

// -------------------------------------------------------------- make-else-part

IfThenNode * This::make_else_part(StmtListNode * stmt_list) noexcept
{
   auto node = unique_ptr<IfThenNode>(new IfThenNode(IfThenType::ELSE));
   vector<AstNode *> children;
   children.reserve(2);
   children.push_back(ExpressionNode::make_empty());
   children.push_back(stmt_list);
   node->set_children(std::move(children));
   return node.release();
}

// -------------------------------------------------------------- make-elif-part

IfThenNode * This::make_elif_part(IfThenType type,
                                  ExpressionNode * condition,
                                  StmtListNode * stmt_list) noexcept
{
   auto node = unique_ptr<IfThenNode>(new IfThenNode(type));
   assert(!node->is_if());
   assert(node->has_condition());   
   vector<AstNode *> children;
   children.reserve(2);
   children.push_back(condition);
   children.push_back(stmt_list);
   node->set_children(std::move(children));
   return node.release();
}

// --------------------------------------------------------------------- getters

const ExpressionNode * This::condition() const noexcept
{
      return cast_ast_node<ExpressionNode>(children()[0]);
}

const StmtListNode *  This::stmts() const noexcept
{
   return cast_ast_node<StmtListNode>(children()[1]);
}

bool This::is_if() const noexcept
{
   return type() == IfThenType::IF
      || type() == IfThenType::IFDEF
      || type() == IfThenType::IFNDEF;
}

bool This::has_condition() const noexcept
{
   return type() != IfThenType::ELSE
      && type() != IfThenType::NONE;
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
   case IfThenType::ELIF:  ss << "#elif"; break;
   case IfThenType::ELIFDEF: ss << "#elifdef"; break;
   case IfThenType::ELIFNDEF: ss << "#elifndef"; break;      
   case IfThenType::ELSE: ss << "#else" ; break;
   }

   // Print the condition
   if(has_condition()) {
      condition()->stream(ss, 0); // Should be on 1 line
   }
   ss << '\n';

   // Print the statement list
   for(const auto stmt: stmts()->children())
      stmt->stream(ss, indent + 1);

   // Print the #elif, #elifdef, #elifndef, #else parts
   for(size_t i = 2; i < children().size(); ++i)
      children()[i]->stream(ss, indent);

   // Print #endif iff #if, #ifdef, #ifndef
   if(is_if()) ss << "#endif" << '\n';
      
   return ss;
}

}
#undef This
