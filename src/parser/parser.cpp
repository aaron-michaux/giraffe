#include "stdinc.hpp"

#include "parser-internal.hpp"

namespace giraffe
{
unique_ptr<TranslationUnitNode> parse(Context& context) noexcept
{
   auto& scanner = context.scanner();

   // Don't worry about TSTART token
   if(scanner.current().id() == TSTART) scanner.consume();

   // Accept and return
   TranslationUnitNode * node = accept_translation_unit(context);
   assert(node != nullptr);
   return unique_ptr<TranslationUnitNode>(node);
}
} // namespace giraffe
