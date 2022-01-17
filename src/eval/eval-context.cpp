#include "stdinc.hpp"

#include <filesystem>

#include "eval-context.hpp"

#include "eval.hpp"
#include "parser/parser.hpp"

#define This EvalContext

namespace giraffe
{

// --------------------------------------------------------------------------------- current context

Context& This::current_context() noexcept
{
   assert(context_stack_.size() > 0);
   return *context_stack_.top();
}

// ---------------------------------------------------------------------------- resolve include path

std::string This::resolve_include_path(string_view filename, bool is_local_include) const noexcept
{
   auto test_it = [&](string_view dir) -> std::pair<std::string, bool> {
      std::string path = format("{}{}{}", dir, (dir.empty() ? "" : "/"), filename);
      return {path, std::filesystem::exists(path)};
   };

   // Test from the local path
   if(is_local_include) {
      auto [path, success] = test_it("");
      if(success) return path;
   }

   // Test from system paths
   for(const auto& dir : driver_opts_.include_paths) {
      auto [path, success] = test_it(dir);
      if(success) return path;
   }

   return ""s;
}

// ------------------------------------------------------------------------ push and process include

void This::process_file(string_view filename) noexcept
{
   assert(std::filesystem::exists(filename));
   context_stack_.push(Context::make(filename, driver_opts_));
   auto& context                           = current_context();
   unique_ptr<TranslationUnitNode> tu_node = parse(context);
   if(!context.has_error()) { eval_node(*this, tu_node.get()); }
   context_stack_.pop();
}

} // namespace giraffe

#undef This
