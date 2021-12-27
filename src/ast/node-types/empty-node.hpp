#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class EmptyNode final : public AstNode
{
private:
   
public:
   EmptyNode() // "file" or <file>. 
      : AstNode(NodeType::EMPTY)
   {}
   virtual ~EmptyNode() = default;
   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override
   {
      for(auto i = 0; i < indent; ++i) ss << ' ';
      ss << "<empty-node>\n";
      return ss;
   }
};

}



