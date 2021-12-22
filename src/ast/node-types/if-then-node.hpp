
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{

enum class IfThenType : uint8_t {
   NONE = 0,
   IF,
   IFDEF,
   IFNDEF,
   ELIF,
   ELSE,
   ENDIF
};

/**
 */
class IfThenNode final : public AstNode
{
private:
   IfThenType type_ = IfThenType::NONE;

public:

   static IfThenNode * make() noexcept;
   
   IfThenNode()
      : AstNode(NodeType::IFTHEN)
   {}
   virtual ~IfThenNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto type() const noexcept { return type_; }
   //@}
};
}

