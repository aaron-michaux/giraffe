#include "stdinc.hpp"

#include "encode-char.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace giraffe
{
std::ostream& encode_char(std::ostream& ss, char val) noexcept
{
   switch(val) {
   case '\t': ss << "\\t"; break;
   case '\n': ss << "\\n"; break;
   case '\v': ss << "\\v"; break;
   case '\f': ss << "\\f"; break;
   case '\r': ss << "\\r"; break;
   case '"': ss << "\\\""; break;
   case '\\': ss << "\\\\"; break;
   default:
      if(std::isprint(val))
         ss << val;
      else
         ss << "\\x" << format("{:02x}", val);
   }
   return ss;
}

std::string encoded_char(char val) noexcept
{
   std::stringstream ss{""};
   encode_char(ss, val);
   return ss.str();
}

std::string encode_string(std::string_view val) noexcept
{
   std::stringstream ss{""};
   for(auto c : val) encode_char(ss, c);
   return ss.str();
}

} // namespace giraffe
