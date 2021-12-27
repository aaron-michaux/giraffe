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
   IncludeNode(string_view filename) // "file" or <file>. 
      : AstNode(NodeType::INCLUDE)
   {
      assert(filename.size() >= 2);
      assert((filename.front() == '"' && filename.back() == '"')
             || (filename.front() == '<' && filename.back() == '>'));
      is_local_include_ = (filename[0] == '"');
      filename_ = string{filename.begin() + 1, filename.begin() + filename.size() - 2};
   }
   virtual ~IncludeNode() = default;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   const auto& filename() const noexcept { return filename_; }
   auto is_local_include() const noexcept { return is_local_include_; }
   //@}
};

}
