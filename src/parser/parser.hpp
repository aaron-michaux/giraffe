
#include "ast/ast.hpp"

namespace giraffe
{
class Context;

unique_ptr<AstNode> parse(Context& context) noexcept;
} // namespace giraffe
