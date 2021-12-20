
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 */
class CommandNode final : public AstNode
{
private:


public:
   CommandNode()
      : AstNode(NodeType::COMMAND)
   {}
   virtual ~CommandNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}
