
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
class StmtListNode final : public AstNode
{
 private:
 public:
   StmtListNode(vector<AstNode*>&& stmts) // "file" or <file>.
       : AstNode(NodeType::STMT_LIST)
   {
      set_children_(std::move(stmts));
   }
   virtual ~StmtListNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override
   {
      for(const auto node : children()) node->stream(ss, indent);
      return ss;
   }
};

} // namespace giraffe
