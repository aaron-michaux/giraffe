#include "stdinc.hpp"

#include "define-node.hpp"

#define This DefineNode

namespace giraffe
{

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

   if(text_.size() > 0) { // Output the text... excaping newline characters
      ss << ' ';
      bool last_ch_was_newline = false;
      for(const auto ch : text_) {
         if(ch == '\n') {
            ss << "\\\n";
            last_ch_was_newline = true;
         } else {
            if(last_ch_was_newline) {
               last_ch_was_newline = false;
               do_indent();
            }
            ss << ch;
         }
      }
   }

   ss << '\n';
   return ss;
}

} // namespace giraffe
#undef This
