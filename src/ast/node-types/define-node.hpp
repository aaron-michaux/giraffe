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
   string identifier_ = {}; 
   string text_ = {};
   vector<string> arglist_ = {};
   
public:
   DefineNode(string_view identifier, string_view text, vector<string> arglist)
      : AstNode(NodeType::DEFINE)
      , identifier_{cbegin(identifier), cend(identifier)}
      , text_{cbegin(text), cend(text)}
      , arglist_{std::move(arglist)}
   {}
   virtual ~DefineNode() = default;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   const auto& identifier() const noexcept { return identifier_; }
   const auto& text() const noexcept { return text_; }
   const auto& arglist() const noexcept { return arglist_; }
   //@}
};

}

