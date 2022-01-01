
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class ErrorNode final : public AstNode
{
 private:
   sso23::string message_ = {};    //
   bool is_error_         = false; // #error #warning

 public:
   ErrorNode(bool is_error, string_view message)
       : AstNode(NodeType::ERROR)
       , message_{message}
       , is_error_{is_error}
   {}
   virtual ~ErrorNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto message() const noexcept { return string_view{message_.data(), message_.size()}; }
   auto is_error() const noexcept { return is_error_; }
   //@}
};

} // namespace giraffe
