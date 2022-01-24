
#pragma once

#include "driver/context.hpp"

namespace giraffe
{
void eval_node(Context& eval_ctx, const AstNode* node) noexcept;
} // namespace giraffe
