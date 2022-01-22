
#pragma once

#include "context.hpp"
#include "diagnostic.hpp"

namespace giraffe
{

std::string render_diagnostic(const Context& context,
                              Diagnostic::Level level,
                              SourceLocation location,
                              SourceRange range,
                              uint32_t carrot,
                              const std::string& message) noexcept;

} // namespace giraffe
