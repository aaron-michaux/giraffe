
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
   vector<sso23::string> arglist_ = {};
   vector<Token> token_sequence_  = {};

 public:
   DefineNode(SourceRange loc,
              string_view identifier,
              vector<sso23::string> arglist,
              vector<Token> token_sequence)
       : AstNode(NodeType::DEFINE, loc)
       , identifier_{identifier}
       , arglist_{std::move(arglist)}
       , token_sequence_{std::move(token_sequence)}
   {}
   virtual ~DefineNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto identifier() const noexcept { return string_view{identifier_.data(), identifier_.size()}; }
   const auto& token_sequence() const noexcept { return token_sequence_; }
   const auto& arglist() const noexcept { return arglist_; }
   //@}
};

} // namespace giraffe
