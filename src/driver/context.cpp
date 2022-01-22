#include "stdinc.hpp"

#include "context.hpp"

#include "format-diagnostic.hpp"

#include "parser/parser.hpp"
#include "scanner/scanner.hpp"

#define This Context

namespace giraffe
{
// --------------------------------------------------------------------------- make-compiler-context

unique_ptr<Context> This::make(unique_ptr<Scanner>&& scanner, DriverOptions opts)
{
   auto context          = unique_ptr<Context>(new Context{});
   context->driver_opts_ = opts;
   context->scanner_stack_.push(std::move(scanner));
   return context;
}

unique_ptr<Context> This::make(string_view filename, DriverOptions opts)
{
   auto scanner = make_unique<Scanner>(make_unique<FILE_ScannerInput>(filename));
   return make(std::move(scanner), opts);
}

// -------------------------------------------------------------------------------- diagnostics from

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

// -------------------------------------------------------------------------------- push diagnostics

void This::push_diagnostic_(Diagnostic::Level level,
                            SourceLocation location,
                            SourceRange rng,
                            std::string&& message) noexcept
{
   if(level == Diagnostic::WARN && driver_opts().w_error) level = Diagnostic::ERROR;
   const uint32_t carrot = 0;
   std::string formatted = render_diagnostic(*this, level, location, rng, carrot, message);
   diags_.push_diagnostic(level, location, rng, std::move(message), std::move(formatted));
}

void This::push_diagnostic_(Diagnostic::Level level,
                            SourceLocation location,
                            std::string&& message) noexcept
{
   push_diagnostic_(level, location, SourceRange{}, std::move(message));
}

void This::push_info(SourceLocation location, SourceRange range, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::INFO, location, range, std::move(message));
}

void This::push_warn(SourceLocation location, SourceRange range, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::WARN, location, range, std::move(message));
}

void This::push_error(SourceLocation location, SourceRange range, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::ERROR, location, range, std::move(message));
}

void This::push_fatal(SourceLocation location, SourceRange range, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::FATAL, location, range, std::move(message));
}

void This::push_info(SourceLocation location, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::INFO, location, std::move(message));
}

void This::push_warn(SourceLocation location, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::WARN, location, std::move(message));
}

void This::push_error(SourceLocation location, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::ERROR, location, std::move(message));
}

void This::push_fatal(SourceLocation location, std::string&& message) noexcept
{
   push_diagnostic_(Diagnostic::FATAL, location, std::move(message));
}

void This::push_info(std::string&& message) noexcept
{
   push_info(scanner().current().location(), std::move(message));
}

void This::push_warn(std::string&& message) noexcept
{
   push_warn(scanner().current().location(), std::move(message));
}

void This::push_error(std::string&& message) noexcept
{
   push_error(scanner().current().location(), std::move(message));
}

void This::push_fatal(std::string&& message) noexcept
{
   push_fatal(scanner().current().location(), std::move(message));
}

// ----------------------------------------------------------------------------------------- actions

ResolvedPath This::resolve_include_path(string_view filename, bool is_local_include) const noexcept
{
   return giraffe::resolve_include_path(include_paths(), filename, is_local_include);
}

void This::process_include(string_view filename, bool is_isystem_path) noexcept
{
   if(scanner_stack_.size() > 100) FATAL(format("suspect infinite #include recusion, aborting!"));
   if(has_error()) return;
   if(driver_opts().skip_system_includes && is_isystem_path) return; // nothing to do

   // Record the include dependency
   include_deps_.insert(sso23::string{filename});

   // Update the options... remember to suppress warnings on -isystem paths
   const auto suppress_warnings = driver_opts().suppress_warnings || is_isystem_path;

   // Process the include file
   assert(std::filesystem::exists(filename));
   auto scanner = make_unique<Scanner>(make_unique<FILE_ScannerInput>(filename));
   scanner_stack_.push(std::move(scanner));

   {
      unique_ptr<TranslationUnitNode> tu_node = parse(*this);

      // Evaluate the node if there's no error
      FATAL("TODO: evaluate the node here");
      // if(!context.has_error()) { eval_node(*this, tu_node.get()); }
   }

   scanner_stack_.pop();
}

// ------------------------------------------------------------------------------------------ stream

std::ostream& This::stream(std::ostream& ss) const noexcept
{
   for(const auto& diagnostic : diagnostics()) ss << diagnostic.to_string();
   return ss;
}

std::string This::to_string() const noexcept
{
   std::stringstream ss{""};
   stream(ss);
   return ss.str();
}

void This::stream_make_rules(std::ostream& os) noexcept
{
   os << "Hello world! We're making Stream rules\n";
}

} // namespace giraffe

#undef This
