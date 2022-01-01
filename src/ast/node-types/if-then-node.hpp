
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{

// [TYPE, condition, command-list]
// #if/#ifdef/#ifndef:
//   child[0] = condition (empty for #else)
//   child[1] = statement-list
//   child[2..n] = #elif, #elifdef, #elifndef, #else parts

enum class IfThenType : uint8_t {
   NONE = 0,
   IF,
   IFDEF,
   IFNDEF,
   ELIF,
   ELIFDEF,
   ELIFNDEF,
   ELSE
};

/**
 */
class IfThenNode final : public AstNode
{
 private:
   IfThenType type_ = IfThenType::NONE;

   IfThenNode(IfThenType type)
       : AstNode(NodeType::IFTHEN)
       , type_(type)
   {}

 public:
   virtual ~IfThenNode() = default;

   static IfThenNode* make_if_part(IfThenType type, vector<AstNode*>&& children) noexcept;
   static IfThenNode* make_elif_part(IfThenType type,
                                     ExpressionNode* condition,
                                     StmtListNode* stmt_list) noexcept;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto type() const noexcept { return type_; }
   const ExpressionNode* condition() const noexcept;
   const StmtListNode* stmts() const noexcept;
   bool is_if() const noexcept;
   bool has_condition() const noexcept;
   //@}
};
} // namespace giraffe
