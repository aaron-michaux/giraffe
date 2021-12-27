#include "stdinc.hpp"

#include "ast/ast.hpp"
#include "translation-unit-node.hpp"

#define This TranslationUnitNode

namespace giraffe {

This::This(StmtListNode * stmts)
      : AstNode(NodeType::TRANSLATION_UNIT)
{
   assert(stmts != nullptr);
   vector<AstNode *> children;
   children.reserve(1);
   children.push_back(stmts); 
   set_children(std::move(children));
}


std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   stmts()->stream(ss, indent);
   return ss;
}

}

#undef This
