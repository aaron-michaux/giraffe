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

   auto print_token = [](auto& os, const Token& token) {
      os << format("{:15s} {:15s} {}\n",
                   str(token.location()),
                   token_id_to_str(token.id()),
                   encode_string(token.text()));
   };

   // There's a 'Context' for every file, but only one 'SymbolTable'
   //
   // (1) Get a stmt list
   // (2) For each statement eval(context,

   auto& scanner = context.scanner();
   while(scanner.has_next()) { print_token(cout, scanner.consume()); }
   print_token(cout, scanner.current());

   return true; // we're winners
}

} // namespace giraffe
