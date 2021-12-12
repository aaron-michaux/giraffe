#include "stdinc.hpp"

#include "execute.hpp"

#include "context.hpp"

#include "parser/parser.hpp"
// #include "sema/sema.hpp"
// #include "term/term.hpp"

namespace giraffe
{
// --------------------------------------------------------------------- execute

int execute(Context& context) noexcept // once set upt
{
   // Statement-by-statement
   // const bool interactive = context.driver_opts().interactive;

   // // Create the interface object...
   // // We'll get the `term` to create
   // ReplInterface interface;
   // interface.prompt           = [&]() { cout << " > " << std::flush; };
   // interface.print_diagnostic = [&](string_view ss) { cout << ss; };
   // interface.echo_function    = [&](const StatementNode* stmt) {
   //    if(is_vvv_mode()) stmt->stream(cout, 0);
   // };

   // read_eval_print_loop(context, interface);

   return true; // we're winners
}

} // namespace giraffe
