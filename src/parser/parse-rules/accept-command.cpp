#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{
CommandNode * accept_command(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   expect(scanner, first_set_command);
   const auto& token = scanner.current();
   const auto id = token.id();

   switch(id) {
   case TDEFINE:
      // #define IDENTIFIER TOKENS*
      // #define IDENTIFIER(identifier-list?) TOKENS*
      
   case TINCLUDE:
      // #include (c-string | <FILENAME>)
      
   case TLINE:
      // #line DIGITS+ c-string?
      
   case TUNDEF:
      // #undef IDENTIFIER
      
   case TERROR:
      // #error TOKENS*
      
   default:
      context.push_error(format("logic error, unexpected token {}", token_id_to_str(id)));
   }
   
   
   //return IfThenNode::make({ begin(children), end(children) });
   return nullptr;
}

}

