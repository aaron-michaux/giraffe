
#pragma once

#include "ast/ast.hpp"
#include "driver/context.hpp"

#include "symbol-table.hpp"

namespace giraffe
{

void eval_stmts(Context& context, SymbolTable& symbols, const StmtListNode* stmt_list) noexcept;

}
