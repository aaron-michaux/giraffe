#include "stdinc.hpp"

#include "translation-unit-node.hpp"

#define This TranslationUnitNode

namespace giraffe {

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   for(auto child: children())
      child->stream(ss, indent);
   return ss;
}


TranslationUnitNode * This::make(vector<AstNode*>&& children) noexcept
{
   auto node = std::make_unique<TranslationUnitNode>();
   node->set_children(std::move(children));
   return node.release();
}

}

#undef This
