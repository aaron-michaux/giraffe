#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class IncludeNode final : public AstNode
{
private:
   string filename_ = {}; // 
   bool is_local_include_ = false;
   
public:
    // "file" or <file>.
   IncludeNode(SourceRange loc, string&& filename, bool is_local_include)
      : AstNode(NodeType::INCLUDE, loc)
      , filename_(std::move(filename)) // file, without "" or <>
      , is_local_include_(is_local_include)
   {}

   virtual ~IncludeNode() = default;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   const auto& filename() const noexcept { return filename_; }
   auto is_local_include() const noexcept { return is_local_include_; }
   //@}
};

}
