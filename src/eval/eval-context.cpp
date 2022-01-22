#include "stdinc.hpp"

#include <filesystem>

#include "eval-context.hpp"

#include "eval.hpp"
#include "parser/parser.hpp"

#define This EvalContext

namespace giraffe
{

// ---------------------------------------------------------------------------------------- evaluate

unique_ptr<EvalContext> This::evaluate(unique_ptr<ScannerInputInterface>&& input,
                                       vector<IncludePath> include_paths,
                                       SymbolTable initial_symbol_table,
                                       DriverOptions opts) noexcept
{
   auto ctx            = make_unique<EvalContext>();
   ctx->include_paths_ = std::move(include_paths);
   ctx->symbols_       = std::move(initial_symbol_table);
   ctx->driver_opts_   = opts;
   ctx->process_context_(Context::make(make_unique<Scanner>(std::move(input)), opts));
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

   // Record the include dependency
   include_deps_.insert(sso23::string{filename});

   // Update the options... remember to suppress warnings on -isystem paths
   auto opts              = driver_opts_;
   opts.suppress_warnings = opts.suppress_warnings || is_isystem_path;

   // Process the include file
   assert(std::filesystem::exists(filename));
   process_context_(Context::make(filename, opts));
}

// --------------------------------------------------------------------------------- process context

void This::process_context_(unique_ptr<Context>&& context_ptr) noexcept
{
   if(context_stack_.size() > 100) {
      FATAL(format("suspect infinite #include recusion, aborting!"));
   }

   context_stack_.push(std::move(context_ptr));

   auto& context                           = current_context();
   unique_ptr<TranslationUnitNode> tu_node = parse(context);

   // Render diagnostics
   for(const auto& diagnostic : context.diagnostics()) {
      std::cerr << diagnostic.to_string();
      switch(diagnostic.level) {
      case Diagnostic::NONE: FATAL("logic error: diagnostic of type NONE"); break;
      case Diagnostic::INFO: [[fallthrough]];
      case Diagnostic::WARN: /* All good */ break;
      case Diagnostic::ERROR: has_error_ = true; break;
      case Diagnostic::FATAL: std::abort();
      }
   }

   // Evaluate the node
   if(!context.has_error()) { eval_node(*this, tu_node.get()); }

   context_stack_.pop();
}

// ------------------------------------------------------------------------------- stream make rules

void This::stream_make_rules(std::ostream& os) noexcept { os << "Hello world!" << endl; }

} // namespace giraffe

#undef This
