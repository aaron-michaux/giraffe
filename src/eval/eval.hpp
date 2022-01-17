
#pragma once

#include "eval-context.hpp"

namespace giraffe
{
void eval_node(EvalContext& eval_ctx, const AstNode* node) noexcept;
} // namespace giraffe
