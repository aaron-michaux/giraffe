
#pragma once

#include "ast/node-types/ast-node.hpp"

// #define IDENTIFIER
// #define IDENTIFIER <text>...
// #define IDENTIFIER(arglist) <text>...

namespace giraffe
{
class DefineNode final : public AstNode
{
 private:
   sso23::string identifier_      = {};
   sso23::string text_            = {};
   vector<sso23::string> arglist_ = {};

 public:
   DefineNode(string_view identifier, string_view text, vector<sso23::string> arglist)
       : AstNode(NodeType::DEFINE)
       , identifier_{identifier}
       , text_{text}
       , arglist_{std::move(arglist)}
   {}
   virtual ~DefineNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto identifier() const noexcept
   {
      return string_view{identifier_.data(), identifier_.size()};
   }
   auto text() const noexcept { return string_view{text_.data(), text_.size()}; }
   const auto& arglist() const noexcept { return arglist_; }
   //@}
};

} // namespace giraffe
