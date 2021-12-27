#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

CommandNode * accept_line(Context& context) noexcept;
CommandNode * accept_undef(Context& context) noexcept;
   

// template<std::integral auto T>
// T str_to_integer(const char * begin, const char * end) noexcept(false)
// {
//    T val = 0;
//    bool is_negative = (*begin == '-');
//    if(is_negative || *begin == '+') ++begin;
   

//    for(auto ii = begin; ii != end; ++ii) {
//       const auto ch = *ii;
//       if(ch == '\'') continue; // Skip ' characters
//       if(!std::isdigit(ch)) {
//          throw std::invalid_argument("expected digit");
//       }
//       val = (val * 10) + (ch - '0'); // overflow errors!
//    }

//    // overflow errors, and type error
//    return (is_negative) ? -val : val;
// }

// ----------------------------------------------------------------- accept-cstr

string accept_cstr(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   expect(scanner, TSTR_DELIM);
   scanner.consume();

   const auto pos0 = scanner.position();
   while(scanner.current().id() == TSTR_PART) 
      scanner.consume();
   const auto pos1 = scanner.position(); // The string is [pos0, pos1)

   if(scanner.current().id() != TSTR_DELIM) {
      context.push_error(format("unexpected token while parsing string"));
      return ""s;
   } else {
      scanner.consume();
   }

   // Consolidate string and return
   auto estimate_length = [&] (const auto pos0, const auto pos1) {
      size_t len = 0;
      for(auto i = pos0; i != pos1; ++i) 
         len += scanner.at(i).text().size();
      scanner.set_position(pos1);
      return len;
   };
   const auto approx_length = estimate_length(pos0, pos1);

   // Append tokens to internal string representation
   auto append = [&] (string& s, const auto& token) {
      const auto text = token.text();
      auto carrot_pos = [&] (auto ii) {
         const auto loc = token.location();
         const auto pos = loc.offset + uint32_t(std::distance(cbegin(text), ii));
         return SourceLocation{pos, loc.line_no};
      };

      auto out_pos = decode_string(s, cbegin(text), cend(text));
      if(out_pos != cend(text)) 
         context.push_error(carrot_pos(out_pos), format("error decoded string"));
   };
   std::string ret;  
   ret.reserve(approx_length);
   for(auto i = pos0; i != pos1; ++i)
      append(ret, scanner.at(i));
   return ret;
}

// ----------------------------------------------------------------- accept-line

CommandNode * accept_include(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   auto on_error = [&] (string&& message) {
      context.push_error(std::move(message));
      skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
      return CommandNode::make_none();
   };
   
   // #include (c-string | <FILENAME>)
   expect(scanner, TINCLUDE);
   scanner.consume();
   const auto& token = scanner.current();
   if(token.id() == TSTRING) {
      scanner.consume();
      return CommandNode::make_include(token.text());
   } else if(token.id() == TSTR_DELIM) {
      return CommandNode::make_include(accept_cstr(context));
   }

   // -- ERROR
   return on_error(format("expected filename after #include"));
}


// ---------------------------------------------------------------- accept-undef

CommandNode * accept_undef(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   // #undef IDENTIFIER
   expect(scanner, TUNDEF);
   scanner.consume();
   const auto& token = scanner.current();
   if(token.id() == TIDENTIFIER) {
      scanner.consume();
      return CommandNode::make_undef(token.text());
   }

   // -- ERROR
   context.push_error(format("expected identifier after #undef"));
   skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
   return CommandNode::make_none();
}

// -------------------------------------------------------------- accept-command

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
            
   case TUNDEF:
      // #undef IDENTIFIER
      return accept_undef(context);
      
   case TERROR:
      // #error TOKENS*
      
   default:
      context.push_error(format("logic error, unexpected token {}", token_id_to_str(id)));
   }
   
   
   //return IfThenNode::make({ begin(children), end(children) });
   return nullptr;
}

}

