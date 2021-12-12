#include "stdinc.hpp"

#include "parser-internal.hpp"

namespace giraffe
{
unique_ptr<AstNode> parse(Context& context) noexcept
{
   return nullptr;
   // if(!expect(context.scanner(), first_set_grammar)) {
   //    context.push_error("expected start token");

   //    // recovery
   //    skip_to_sequence(context.scanner(), first_set_grammar);
   // }

   // if(expect(context.scanner(), first_set_grammar))
   //    return unique_ptr<GrammarNode>(accept_grammar(context));
   // return make_unique<GrammarNode>();
}
} // namespace giraffe
