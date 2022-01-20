#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

// '(' IDENTIFIER (',' IDENTIFIER)* ')'
vector<sso23::string> accept_arglist(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   vector<sso23::string> args;

   auto push_arg = [&args](const auto text) { args.emplace_back(text.begin(), text.end()); };

   assert(expect(scanner, TLPAREN));
   scanner.consume(); // '('
   bool is_first = true;
   while(true) {
      const auto id = scanner.current().id();

      if(id == TRPAREN) {
         scanner.consume();
         break; // all is good

      } else if(id == TIDENTIFIER && args.size() == 0) {
         push_arg(scanner.consume().text());

      } else if(id == TCOMMA && args.size() > 0) {
         scanner.consume(); // The comma
         if(scanner.current().id() == TIDENTIFIER) {
            push_arg(scanner.consume().text());
         } else {
            context.push_error("unexpected token");
            return args;
         }

      } else {
         context.push_error("unexpected token");
         return args;
      }
   }

   return args;
}

// #define IDENTIFIER
// #define IDENTIFIER <text>...
// #define IDENTIFIER(arglist) <text>...
AstNode* accept_define(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   auto on_error = [&](std::string&& message) {
      context.push_error(std::move(message));
      skip_to_sequence(scanner, TNEWLINE); // Skip to newline
      return make_empty_node();
   };

   assert(expect(scanner, TDEFINE));
   const auto loc0         = scanner.consume().location();
   const auto& ident_token = scanner.current();
   if(ident_token.id() != TIDENTIFIER) { return on_error("expected identifier after #define"); }
   const auto loc1 = scanner.consume().end_location(); // identifier

   vector<sso23::string> arglist = {};
   if(scanner.current().id() == TLPAREN) {
      // Check to see if there's white-space between the current token and the previous identifier
      const auto paren_loc = scanner.current().location();
      if(paren_loc == loc1) arglist = accept_arglist(context);
   }

   // Grab the token sequence to "new line"
   const auto pos1    = scanner.position();
   sso23::string text = accept_to_newline(context);
   const auto pos2    = scanner.position();
   vector<Token> token_sequence;
   token_sequence.reserve(pos2 - pos1);
   for(auto i = pos1; i < pos2; ++i) token_sequence.push_back(scanner.at(i));

   return new DefineNode{
       {loc0, loc1}, ident_token.text(), std::move(arglist), std::move(token_sequence)};
}

} // namespace giraffe
