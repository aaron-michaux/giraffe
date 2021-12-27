#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

string accept_to_newline(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   // Consume tokens until a newline character
   const auto pos0 = scanner.position();
   size_t approx_len = 0;
   while(true) {
      const auto& token = scanner.current();
      if(token.id() == TEOF || token.id() == TNEWLINE)
         break;
      approx_len += token.text().size();
   }
   const auto pos1 = scanner.position(); // text = [pos0, pos1)

   // Concatenate the tokens
   auto append = [] (auto& dst, const auto& s) {
      dst.insert(end(dst), s.begin(), s.end());
   };
   string text = {};
   text.reserve(approx_len);
   for(auto i = pos0; i < pos1; ++i)
      append(text, scanner.at(i).text());

   return text;
}


}
