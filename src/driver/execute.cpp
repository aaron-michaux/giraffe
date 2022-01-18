#include "stdinc.hpp"

#include "context.hpp"

#include "eval/eval-context.hpp"

namespace giraffe
{
// ----------------------------------------------------------------------------------------- execute

bool execute(unique_ptr<ScannerInputInterface>&& input,
             vector<IncludePath> include_paths,
             SymbolTable initial_symbol_table,
             std::ostream* output_stream,
             DriverOptions opts) noexcept // once set upt
{
   // 1. Create eval-context
   // 2. Evaluate input source through eval-context
   auto ctx = EvalContext::make(
       std::move(include_paths), std::move(initial_symbol_table), output_stream, opts);

   // Process the input...
   ctx->process_input(std::move(input));

   // Return if there was an error
   return !ctx->has_error();
}

} // namespace giraffe
