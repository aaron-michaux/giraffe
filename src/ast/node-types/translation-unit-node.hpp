
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 * Just the skeleton of the TranslationUnit: relevant preprocessor commands, and 
 * module keywords
 */
class TranslationUnitNode final : public AstNode
{
private:


public:
   TranslationUnitNode()
      : AstNode(NodeType::TRANSLATION_UNIT)
   {}
   virtual ~TranslationUnitNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   //@}
};
}

