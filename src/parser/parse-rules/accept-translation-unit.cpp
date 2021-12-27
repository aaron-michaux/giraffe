#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
TranslationUnitNode * accept_translation_unit(Context& context) noexcept
{
   unique_ptr<StmtListNode> stmts{accept_stmt_list(context)};
   assert(stmts != nullptr);
   return new TranslationUnitNode{stmts.release()};
}
}

