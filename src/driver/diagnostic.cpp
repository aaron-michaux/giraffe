#include "stdinc.hpp"

#include "context.hpp"
#include "diagnostic.hpp"
#include "utils/in-list.hpp"

namespace giraffe
{
std::ostream& Diagnostic::stream(std::ostream& ss,
                                 const Context& context) const noexcept
{
   static constexpr auto sentence_ends = to_array<char>({'.', '!', '?'});

   const bool use_color = context.driver_opts().color_diagnostics;

   const auto ldat   = context.scanner().line_data(location.line_no);
   const auto line   = ldat.line;
   const auto col_no = location.offset - ldat.offset;

   ss << context.scanner().name();
   ss << ':' << (location.line_no + 1) << ':' << col_no << ": ";
   if(use_color) { ss << k_level_colors.at(size_t(level)); }
   ss << k_level_names.at(size_t(level)) << ':';
   if(use_color) { ss << k_color_none; }
   ss << ' ' << message;
   if(message.size() > 0 && !in_list(message.back(), sentence_ends)) ss << '.';
   ss << '\n';

   auto print_carrot = [&]() {
      if(use_color) { ss << k_light_green; }
      ss << '^';
      if(use_color) { ss << k_color_none; }
   };

   auto print_highlight = [&](const auto ncols) {
      if(use_color) { ss << k_color_white; }
      for(size_t i = 0; i < ncols; ++i) ss << '~';
      if(use_color) { ss << k_color_none; }
   };

   auto print_space = [&](const auto ncols) {
      for(size_t i = 0; i < ncols; ++i) ss << ' ';
   };

   if(line.size() > 0) { // the string is non-empty
      // The line of code
      ss << line;
      if(line.back() != '\n') ss << '\n';

      if(is_empty(range)) {
         print_space(col_no);
         print_carrot(); // The carrot
      } else {
         assert(location.line_no >= range.first.line_no);
         assert(location.line_no <= range.second.line_no);
         const auto carrot_col = col_no;
         const auto col0       = (location.line_no == range.first.line_no
                                      ? range.first.offset - ldat.offset
                                      : 0);
         const auto col1       = (location.line_no == range.first.line_no
                                      ? range.second.offset - ldat.offset
                                      : ldat.line.size());
         if(carrot_col < col0) {
            print_space(carrot_col);
            print_carrot();
            print_space(col0 - carrot_col - 1);
            print_highlight(col1 - col0);
         } else if(carrot_col >= col1) {
            print_space(col0);
            print_highlight(col1 - col0);
            print_space(carrot_col - col1);
            print_carrot();
         } else {
            print_space(col0);
            print_highlight(carrot_col - col0);
            print_carrot();
            print_highlight(col1 - carrot_col - 1);
         }
      }

      ss << '\n';
   }

   return ss;
}

// ------------------------------------------------------------ push diagnostics
// May rewrite this to a generic "clear" function,
// that also flushes out unused data in the scanner.
void Diagnostics::push_diagnostic(Diagnostic::Level level,
                                  SourceLocation location,
                                  SourceRange range,
                                  string&& message) noexcept
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

   diagnostics_.emplace_back(level, location, range, std::move(message));
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

string Diagnostics::to_string(const Context& context) const noexcept
{
   std::stringstream ss{""};
   for(const auto& diagnostic : diagnostics_) diagnostic.stream(ss, context);
   return ss.str();
}

} // namespace giraffe
