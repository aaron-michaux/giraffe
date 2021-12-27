
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
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}
