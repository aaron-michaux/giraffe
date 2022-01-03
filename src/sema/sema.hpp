
#pragma once

#include "ast/ast.hpp"

/**
 * All expressions evaluate to an integer type of some form
 */
Integer evaluate_expr(const ExpressionNode* expr) noexcept(false);

} // namespace giraffe
