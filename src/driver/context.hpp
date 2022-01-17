
#pragma once

#include "diagnostic.hpp"
#include "driver-options.hpp"
#include "scanner/scanner.hpp"

namespace giraffe
{
struct Token;
class Scope;

/**
 * The compiler context has the following details:
 * [1] Any command line options (compiler/scanner opts).
 * [2] The text data of all input buffers.
 * [3] The scanner which produces the token sequence.
 * [4] Any diagnostics generated anywhere in the compiler.
 */
class Context final
{
 private:
   ScannerOptions scanner_opts_ = {};
   DriverOptions driver_opts_   = {};
   Diagnostics diags_           = {};
   unique_ptr<Scanner> scanner_ = {};

   void push_diagnostic_(Diagnostic::Level, SourceLocation, SourceRange, std::string&&) noexcept;
   void push_diagnostic_(Diagnostic::Level, SourceLocation, std::string&&) noexcept;

   Context() = default;

 public:
   Context(const Context&) = delete;
   Context(Context&&)      = default;
   ~Context()              = default;
   Context& operator=(const Context&) = delete;
   Context& operator=(Context&&) = default;

   //@{ Construction
   static unique_ptr<Context> make(unique_ptr<Scanner>&&, DriverOptions opts = {});
   static unique_ptr<Context> make(string_view filename, DriverOptions opts = {});
   //@}

   //@{ Getters
   const auto& scanner_opts() const noexcept { return scanner_opts_; }
   const auto& driver_opts() const noexcept { return driver_opts_; }
   const auto& scanner() const noexcept { return *scanner_; }
   auto& scanner() noexcept { return *scanner_.get(); }
   const auto& diagnostics() const noexcept { return diags_; }
   Diagnostics::Range diagnostics_from(uint32_t) const noexcept;
   bool has_error(Diagnostics::Range = {}) const noexcept;
   //@}

   //@{ Pushing Diagnostics
   void push_info(SourceLocation, SourceRange, std::string&& message) noexcept;
   void push_warn(SourceLocation, SourceRange, std::string&& message) noexcept;
   void push_error(SourceLocation, SourceRange, std::string&& message) noexcept;
   void push_fatal(SourceLocation, SourceRange, std::string&& message) noexcept;

   void push_info(SourceLocation location, std::string&& message) noexcept;
   void push_warn(SourceLocation location, std::string&& message) noexcept;
   void push_error(SourceLocation location, std::string&& message) noexcept;
   void push_fatal(SourceLocation location, std::string&& message) noexcept;

   // Uses the location of the current token
   void push_info(std::string&& message) noexcept;
   void push_warn(std::string&& message) noexcept;
   void push_error(std::string&& message) noexcept;
   void push_fatal(std::string&& message) noexcept;
   //@}

   std::ostream& stream(std::ostream&) const noexcept;
   std::string to_string() const noexcept;
};

} // namespace giraffe
