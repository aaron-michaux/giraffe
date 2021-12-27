
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 */
class ExpressionNode final : public AstNode
{
private:


public:
   ExpressionNode()
      : AstNode(NodeType::EXPRESSION)
   {}
   virtual ~ExpressionNode() = default;

   static ExpressionNode * make_empty() noexcept { return new ExpressionNode{}; }
   static ExpressionNode * make_identifier(const SourceLocation op_loc,
                                           string&& identifier) noexcept;
   static ExpressionNode * make_integer(const SourceLocation op_loc,
                                        string&& integer_str) noexcept;
   static ExpressionNode * make_subexpr(const SourceRange expr_range,
                                        ExpressionNode * subexpr) noexcept;
   static ExpressionNode * make_unary(const int op,
                                      const SourceLocation op_loc,
                                      const SourceRange expr_range,
                                      ExpressionNode* expr) noexcept;
   static ExpressionNode * make_binary(const int op,
                                       const SourceLocation op_loc,
                                       const SourceRange expr_range,
                                       ExpressionNode* lhs,
                                       ExpressionNode* rhs) noexcept;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}
