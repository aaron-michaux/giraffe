
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

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}
