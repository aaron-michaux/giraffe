#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class UndefNode final : public AstNode
{
private:
   string identifier_ = {}; // 
   
public:
   UndefNode(string_view identifier)
      : AstNode(NodeType::UNDEF), identifier_{cbegin(identifier), cend(identifier)}
   {}
   virtual ~UndefNode() = default;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   const auto& identifier() const noexcept { return identifier_; }
   //@}
};

}
