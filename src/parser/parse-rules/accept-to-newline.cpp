#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

string accept_to_newline(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   // Consume tokens until a newline character
   const auto pos0   = scanner.position();
   size_t approx_len = 0;
   while(true) {
      const auto& token = scanner.current();
      if(token.id() == TEOF || token.id() == TNEWLINE) break;
      scanner.consume();
      approx_len += token.text().size();
   }
   const auto pos1 = scanner.position(); // text = [pos0, pos1)
   if(pos0 == pos1) { return ""s; }

   // Concatenate the tokens
   auto append
       = [](auto& dst, const auto& s) { dst.insert(end(dst), s.begin(), s.end()); };
   string text = {};
   text.reserve(size_t(approx_len * 1.25));
   append(text, scanner.at(pos0).text());
   for(auto i = pos0 + 1; i < pos1; ++i) { //
      const auto loc0 = scanner.at(i - 1).end_location();
      const auto loc1 = scanner.at(i - 0).location();
      if(loc0.line_no == loc1.line_no)
         for(auto j = loc0.offset; j < loc1.offset; ++j) text.push_back(' ');
      append(text, scanner.at(i).text());
   }

   return text;
}

} // namespace giraffe
