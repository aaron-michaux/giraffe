
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 */
class ModuleNode final : public AstNode
{
private:


public:
   ModuleNode()
      : AstNode(NodeType::MODULE)
   {}
   virtual ~ModuleNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}

