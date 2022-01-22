#include "stdinc.hpp"

#include "context.hpp"
#include "diagnostic.hpp"
#include "utils/in-list.hpp"

namespace giraffe
{

// ------------------------------------------------------------ push diagnostics
// May rewrite this to a generic "clear" function,
// that also flushes out unused data in the scanner.
void Diagnostics::push_diagnostic(Diagnostic::Level level,
                                  SourceLocation location,
                                  SourceRange range,
                                  std::string&& message,
                                  std::string&& formatted) noexcept
{
   switch(level) {
   case Diagnostic::NONE:
      assert(false);
      std::abort();
      break;
   case Diagnostic::INFO: ++totals_.infos; break;
   case Diagnostic::WARN: ++totals_.warns; break;
   case Diagnostic::ERROR: ++totals_.errors; break;
   case Diagnostic::FATAL: ++totals_.fatals; break;
   }

   diagnostics_.emplace_back(level, location, range, std::move(message), std::move(formatted));
}

// ---------------------------------------------------------------------- totals

DiagnosticCounts Diagnostics::totals(Range ij) const noexcept
{
   DiagnosticCounts counts;

   ij.second = std::max(ij.second, uint32_t(diagnostics_.size()));

   for(auto i = ij.first; i < ij.second; ++i) {
      const auto& diagnostic = diagnostics_[i];
      switch(diagnostic.level) {
      case Diagnostic::NONE:
         assert(false);
         std::abort();
         break;
      case Diagnostic::INFO: ++counts.infos; break;
      case Diagnostic::WARN: ++counts.warns; break;
      case Diagnostic::ERROR: ++counts.errors; break;
      case Diagnostic::FATAL: ++counts.fatals; break;
      }
   }

   return counts;
}

// ------------------------------------------------------------------- to-string

std::string Diagnostics::to_string() const noexcept
{
   std::stringstream ss{""};
   for(const auto& diagnostic : diagnostics_) ss << diagnostic.formatted;
   return ss.str();
}

} // namespace giraffe
