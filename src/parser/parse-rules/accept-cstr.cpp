#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

// ----------------------------------------------------------------- accept-cstr

std::pair<string, SourceRange> accept_cstr(Context& context) noexcept
{
   Scanner& scanner = context.scanner();

   assert(expect(scanner, TSTR_DELIM));
   const auto loc0 = scanner.consume().location();

   const auto pos0 = scanner.position();
   while(scanner.current().id() == TSTR_PART) scanner.consume();
   const auto pos1 = scanner.position(); // The string is [pos0, pos1)

   if(scanner.current().id() != TSTR_DELIM) {
      context.push_error(format("unexpected token while parsing string"));
      return {""s, {}};
   }
   const auto loc1 = scanner.consume().location();

   // Consolidate string and return
   auto estimate_length = [&](const auto pos0, const auto pos1) {
      size_t len = 0;
      for(auto i = pos0; i != pos1; ++i) len += scanner.at(i).text().size();
      scanner.set_position(pos1);
      return len;
   };
   const auto approx_length = estimate_length(pos0, pos1);

   // Append tokens to internal string representation
   auto append = [&](string& s, const auto& token) {
      const auto text = token.text();
      auto carrot_pos = [&](auto ii) {
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
   for(auto i = pos0; i != pos1; ++i) append(ret, scanner.at(i));
   return {ret, {loc0, loc1}};
}

} // namespace giraffe
