#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class ErrorNode final : public AstNode
{
private:
   string message_ = {}; //
   bool is_error_ = false; // #error #warning
   
public:
   ErrorNode(bool is_error, string message)
      : AstNode(NodeType::ERROR)
      , message_{std::move(message)}
      , is_error_{is_error}
   {}
   virtual ~ErrorNode() = default;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   const auto& message() const noexcept { return message_; }
   auto is_error() const noexcept { return is_error_; }
   //@}
};

}
