
#pragma once

#include "ast/ast.hpp"
#include "driver/context.hpp"

#include "integer.hpp"

namespace giraffe
{

/**
 * Takes an integer literal (as text), and returns an Integer object
 */
Integer parse_integer(string_view text) noexcept(false);

/**
 * All expressions evaluate to an integer type of some form
 */
Integer evaluate_expr(const Context& ctx, const ExpressionNode* expr) noexcept(false);

} // namespace giraffe
