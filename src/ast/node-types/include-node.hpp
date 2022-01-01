#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class IncludeNode final : public AstNode
{
 private:
   sso23::string filename_ = {}; //
   bool is_local_include_  = false;

 public:
   // "file" or <file>.
   IncludeNode(SourceRange loc, string_view filename, bool is_local_include)
       : AstNode(NodeType::INCLUDE, loc)
       , filename_(filename) // file, without "" or <>
       , is_local_include_(is_local_include)
   {}

   virtual ~IncludeNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto filename() const noexcept
   {
      return string_view{filename_.data(), filename_.size()};
   }
   auto is_local_include() const noexcept { return is_local_include_; }
   //@}
};

} // namespace giraffe
