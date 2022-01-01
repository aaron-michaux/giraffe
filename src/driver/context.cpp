#include "stdinc.hpp"

#include "context.hpp"
#include "parser/parser.hpp"
#include "scanner/scanner.hpp"

#define This Context

namespace giraffe
{
// ------------------------------------------------------- make-compiler-context

unique_ptr<Context> This::make(unique_ptr<Scanner>&& scanner, DriverOptions opts)
{
   auto context          = unique_ptr<Context>(new Context{});
   context->driver_opts_ = opts;
   context->scanner_     = std::move(scanner);
   return context;
}

// ------------------------------------------------------------ diagnostics from

Diagnostics::Range This::diagnostics_from(uint32_t from_idx) const noexcept
{
   assert(from_idx <= uint32_t(diags_.size()));
   return {from_idx, uint32_t(diags_.size())};
}

bool This::has_error(Diagnostics::Range range) const noexcept
{
   const auto tots = this->diags_.totals(range);
   return tots.fatals > 0 || tots.errors > 0;
}

// ------------------------------------------------------------ push diagnostics

void This::push_diagnostic_(Diagnostic::Level level,
                            SourceLocation location,
                            SourceRange rng,
                            string&& message) noexcept
{
   if(level == Diagnostic::WARN && driver_opts().w_error) level = Diagnostic::ERROR;
   diags_.push_diagnostic(level, location, rng, std::move(message));
}

void This::push_diagnostic_(Diagnostic::Level level,
                            SourceLocation location,
                            string&& message) noexcept
{
   push_diagnostic_(level, location, SourceRange{}, std::move(message));
}

void This::push_info(SourceLocation location,
                     SourceRange range,
                     string&& message) noexcept
{
   push_diagnostic_(Diagnostic::INFO, location, range, std::move(message));
}

void This::push_warn(SourceLocation location,
                     SourceRange range,
                     string&& message) noexcept
{
   push_diagnostic_(Diagnostic::WARN, location, range, std::move(message));
}

void This::push_error(SourceLocation location,
                      SourceRange range,
                      string&& message) noexcept
{
   push_diagnostic_(Diagnostic::ERROR, location, range, std::move(message));
}

void This::push_fatal(SourceLocation location,
                      SourceRange range,
                      string&& message) noexcept
{
   push_diagnostic_(Diagnostic::FATAL, location, range, std::move(message));
}

void This::push_info(SourceLocation location, string&& message) noexcept
{
   push_diagnostic_(Diagnostic::INFO, location, std::move(message));
}

void This::push_warn(SourceLocation location, string&& message) noexcept
{
   push_diagnostic_(Diagnostic::WARN, location, std::move(message));
}

void This::push_error(SourceLocation location, string&& message) noexcept
{
   push_diagnostic_(Diagnostic::ERROR, location, std::move(message));
}

void This::push_fatal(SourceLocation location, string&& message) noexcept
{
   push_diagnostic_(Diagnostic::FATAL, location, std::move(message));
}

void This::push_info(string&& message) noexcept
{
   push_info(scanner().current().location(), std::move(message));
}

void This::push_warn(string&& message) noexcept
{
   push_warn(scanner().current().location(), std::move(message));
}

void This::push_error(string&& message) noexcept
{
   push_error(scanner().current().location(), std::move(message));
}

void This::push_fatal(string&& message) noexcept
{
   push_fatal(scanner().current().location(), std::move(message));
}

// ---------------------------------------------------------------------- stream

std::ostream& This::stream(std::ostream& ss) const noexcept
{
   for(const auto& diagnostic : diagnostics()) diagnostic.stream(ss, *this);
   return ss;
}

string This::to_string() const noexcept
{
   std::stringstream ss{""};
   stream(ss);
   return ss.str();
}

} // namespace giraffe

#undef This
