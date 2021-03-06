
#pragma once

#include "scanner/source-location.hpp"

namespace giraffe
{
class Context;

struct Diagnostic final
{
 public:
   enum Level : uint8_t {
      NONE = 0,
      INFO,
      WARN,
      ERROR, // attempts to recover
      FATAL  // halts parsing
   };

   static constexpr auto k_color_none  = string_view("\x1b[0m");
   static constexpr auto k_light_green = string_view("\x1b[92m");
   static constexpr auto k_color_white = string_view("\x1b[97m");
   static constexpr auto k_level_colors
       = to_array<string_view>({"\x1b[0m", "\x1b[34m", "\x1b[33m", "\x1b[91m", "\x1b[41m\x1b[97m"});
   static constexpr auto k_level_names
       = to_array<string_view>({"none", "info", "warning", "error", "fatal"});

   std::string message     = ""s; //!< The actual error message
   std::string formatted   = ""s; //!< The full diagnostic, formatted with the relevant line of text
   SourceLocation location = {};
   SourceRange range       = {};
   uint32_t length         = 0; // from `loc`
   uint32_t carrot         = 0; // The highlight point
   Level level             = NONE;

   Diagnostic()                  = default;
   Diagnostic(const Diagnostic&) = default;
   Diagnostic(Diagnostic&&)      = default;
   ~Diagnostic()                 = default;
   Diagnostic& operator=(const Diagnostic&) = default;
   Diagnostic& operator=(Diagnostic&&) = default;

   Diagnostic(Level lv,
              SourceLocation loc,
              SourceRange rng,
              std::string&& msg,
              std::string&& formatted_diagnostic) noexcept
       : message(std::move(msg))
       , formatted(std::move(formatted_diagnostic))
       , location(loc)
       , range(rng)
       , level(lv)
   {
      if(!is_empty(range)) {
         assert(location.line_no >= range.first.line_no);
         assert(location.line_no <= range.second.line_no);
      }
   }

   const auto& to_string() const noexcept { return formatted; }
};

struct DiagnosticMessage final
{
   std::string message     = {};               //!< The formated message
   Diagnostic::Level level = Diagnostic::NONE; //!< Info/Warn/etc.
};

struct DiagnosticCounts
{
   uint32_t infos  = 0;
   uint32_t warns  = 0;
   uint32_t errors = 0;
   uint32_t fatals = 0;
};

class Diagnostics
{
 public:
   using Range = std::pair<uint32_t, uint32_t>;

 private:
   DiagnosticCounts totals_        = {};
   vector<Diagnostic> diagnostics_ = {};

 public:
   void push_diagnostic(Diagnostic::Level,
                        SourceLocation,
                        SourceRange,
                        std::string&&,
                        std::string&&) noexcept;

   size_t size() const noexcept { return diagnostics_.size(); }

   const auto& totals() const noexcept { return totals_; }

   DiagnosticCounts totals(Range) const noexcept;

   const auto& diagnostics() const noexcept { return diagnostics_; }

   auto begin() const noexcept { return diagnostics_.begin(); }
   auto rbegin() const noexcept { return diagnostics_.rbegin(); }
   auto end() const noexcept { return diagnostics_.end(); }
   auto rend() const noexcept { return diagnostics_.rend(); }
   auto cbegin() const noexcept { return diagnostics_.cbegin(); }
   auto crbegin() const noexcept { return diagnostics_.crbegin(); }
   auto cend() const noexcept { return diagnostics_.cend(); }
   auto crend() const noexcept { return diagnostics_.crend(); }

   std::string to_string() const noexcept;
};

} // namespace giraffe
