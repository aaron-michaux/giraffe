
#pragma once

#include "ast/ast.hpp"

namespace giraffe
{
class Context;
unique_ptr<TranslationUnitNode> parse(Context& context) noexcept;
} // namespace giraffe
