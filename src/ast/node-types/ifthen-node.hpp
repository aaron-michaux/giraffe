
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 */
class IfThenNode final : public AstNode
{
private:


public:
   IfThenNode()
      : AstNode(NodeType::IFTHEN)
   {}
   virtual ~IfThenNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}

