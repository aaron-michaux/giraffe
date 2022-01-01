
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class UndefNode final : public AstNode
{
 private:
   sso23::string identifier_ = {}; //

 public:
   UndefNode(string_view identifier)
       : AstNode(NodeType::UNDEF)
       , identifier_{identifier.begin(), identifier.end()}
   {}
   virtual ~UndefNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto identifier() const noexcept
   {
      return string_view{identifier_.data(), identifier_.size()};
   }
   //@}
};

} // namespace giraffe
