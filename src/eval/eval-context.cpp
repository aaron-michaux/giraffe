#include "stdinc.hpp"

#include <filesystem>

#include "eval-context.hpp"

#include "eval.hpp"
#include "parser/parser.hpp"

#define This EvalContext

namespace giraffe
{

// -------------------------------------------------------------------------------------------- make

unique_ptr<EvalContext> This::make(vector<IncludePath> include_paths,
                                   SymbolTable initial_symbol_table,
                                   std::ostream* output_stream,
                                   DriverOptions opts) noexcept
{
   auto ctx            = make_unique<EvalContext>();
   ctx->include_paths_ = std::move(include_paths);
   ctx->symbols_       = std::move(initial_symbol_table);
   ctx->ostream_       = output_stream;
   ctx->driver_opts_   = opts;
   return ctx;
}

// --------------------------------------------------------------------------------- current context

Context& This::current_context() noexcept
{
   assert(context_stack_.size() > 0);
   return *context_stack_.top();
}

// ---------------------------------------------------------------------------- resolve include path

ResolvedPath This::resolve_include_path(string_view filename, bool is_local_include) const noexcept
{
   ResolvedPath ret = {};

   auto test_it = [&filename, &ret](string_view dir, bool is_isystem) {
      std::string path = format("{}{}{}", dir, (dir.empty() ? "" : "/"), filename);
      if(std::filesystem::exists(path)) {
         ret.filename        = std::move(path);
         ret.is_found        = true;
         ret.is_isystem_path = is_isystem;
      }
   };

   // Test for local include if `is_local_include`
   if(is_local_include) test_it("", false);

   // Test other paths
   for(auto ii = begin(include_paths_); !ret.is_found && ii != end(include_paths_); ++ii)
      test_it(ii->path, ii->is_isystem);

   return ret;
}

// --------------------------------------------------------------------------------- process include

void This::process_include(string_view filename, bool is_isystem_path) noexcept
{
   if(driver_opts_.skip_system_includes && is_isystem_path) {
      return; // nothing to do
   }

   // Update the options... remember to suppress warnings on -isystem paths
   auto opts              = driver_opts_;
   opts.suppress_warnings = opts.suppress_warnings || is_isystem_path;

   assert(std::filesystem::exists(filename));
   process_context_(Context::make(filename, opts));
}

void This::process_input(unique_ptr<ScannerInputInterface>&& input) noexcept
{
   process_context_(Context::make(make_unique<Scanner>(std::move(input)), driver_opts_));
}

void This::process_context_(unique_ptr<Context>&& context_ptr) noexcept
{
   context_stack_.push(std::move(context_ptr));

   auto& context                           = current_context();
   unique_ptr<TranslationUnitNode> tu_node = parse(context);

   // Render
   for(const auto& diagnostic : context.diagnostics()) {
      diagnostic.stream(std::cerr, context);
      switch(diagnostic.level) {
      case Diagnostic::NONE: FATAL("logic error: diagnostic of type NONE"); break;
      case Diagnostic::INFO: [[fallthrough]];
      case Diagnostic::WARN: /* All good */ break;
      case Diagnostic::ERROR: has_error_ = true; break;
      case Diagnostic::FATAL: std::abort();
      }
   }

   if(!context.has_error()) { eval_node(*this, tu_node.get()); }

   if(!context.has_error()) {
      // TODO, stream output dependencies here (if
   }

   context_stack_.pop();
}

} // namespace giraffe

#undef This
