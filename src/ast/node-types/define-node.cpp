#include "stdinc.hpp"

#include "define-node.hpp"

#define This DefineNode

namespace giraffe
{

std::ostream&
output_white_space_between(std::ostream& os, SourceLocation start, SourceLocation finish)
{
   if(start.line_no != finish.line_no) os << ' ';
   for(auto line_no = start.line_no; line_no < finish.line_no; ++line_no) os << "\\\n";
   for(auto pos = start.offset; pos < finish.offset; ++pos) os << ' ';
   return os;
}

template<typename T> std::ostream& stream_tokens(std::ostream& os, T start, T finish)
{
   if(start == finish) return os; // Trivial case

   // Output the first token
   encode_string(os, start->text());

   // For each adjacent pair,
   //  1. Output the white space between the two tokens
   //  2. Output the second token
   for_each_adjacent_pair(start, finish, [&](T ii, T jj) {
      output_white_space_between(os, ii->end_location(), jj->location());
      encode_string(os, jj->text());
   });

   return os;
}

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   auto do_indent = [indent, &ss]() {
      for(auto i = 0; i < indent; ++i) ss << ' ';
   };
   do_indent();

   ss << "#define " << identifier();
   if(arglist_.size() > 0) { // Output the argument list
      ss << '(';
      bool first_arg = true;
      for(const auto& arg : arglist_) {
         if(first_arg)
            first_arg = false;
         else
            ss << ", ";
         ss << arg;
      }
      ss << ')';
   }

   if(token_sequence().size() > 0) { // Output the text... excaping newline characters
      ss << ' ';
      stream_tokens(ss, cbegin(token_sequence()), cend(token_sequence()));
   }

   ss << '\n';
   return ss;
}

} // namespace giraffe
#undef This
