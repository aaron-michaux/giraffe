
#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "fmt/format.h"

namespace giraffe
{
inline std::ostream& encode_char(std::ostream& ss, char val) noexcept
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
         ss << "\\x" << fmt::format("{:02x}", val);
   }
   return ss;
}

inline std::string encoded_char(char val) noexcept
{
   std::stringstream ss{""};
   encode_char(ss, val);
   return ss.str();
}

inline std::string encode_string(std::string_view val) noexcept
{
   std::stringstream ss{""};
   for(auto c : val) encode_char(ss, c);
   return ss.str();
}


/**
 * @return The iterator position where decodeing stops... i.e., end if no error
 */
inline auto decode_string(std::string& s, auto begin, auto end) noexcept
{
   auto decode_octal_sequence = [&end] (auto& ii) -> char {
      // octal sequence, [0-7]{1,3}
      int val = *ii++ - '0';
      for(auto i = 1; i < 3 && ii != end && *ii >= '0' && *ii <= '7'; ++i) {
         val *= 8; // base 8
         val += *ii++ - '0'; // ascii to integer
      }
      return char(val);
   };

   auto decode_hexidecimal_sequence = [&end] (auto& ii) -> char {
      auto convert_hex_digit = [] (char val) -> int {
         if(val >= '0' && val <= '9') return val - '0';
         if(val >= 'a' && val <= 'f') return 10 + val - 'a';
         return 10 + val - 'A'; 
      };      
      int val = convert_hex_digit(*ii++);
      while(ii != end && std::isxdigit(*ii))
         val = (val * 16) + convert_hex_digit(*ii++);
      return char(val);
   };

   
   for(auto ii = begin; ii != end; ++ii) {
      const char ch = *ii;
      if(ch != '\\') {
         s.push_back(ch);
         continue;
      }

      auto jj = std::next(ii);
      if(jj == end) {
         return ii; // ERROR!
      } else {
         ii = jj; // get the text character
      }

      switch(*ii) {
      case '\'': [[fallthrough]];
      case '"': [[fallthrough]];
      case '?': [[fallthrough]];
      case '\\':
         s.push_back(*ii);
         break;
         
      case 'a':
         s.push_back('\a');
         break;
      case 'b':
         s.push_back('\b');
         break;
      case 'f':
         s.push_back('\f');
         break;
      case 'n':
         s.push_back('\n');
         break;
      case 'r':
         s.push_back('\r');
         break;
      case 't':
         s.push_back('\t');
         break;
      case 'v':
         s.push_back('\v');
         break;
         
      case '0':[[fallthrough]];
      case '1':[[fallthrough]];
      case '2':[[fallthrough]];
      case '3':[[fallthrough]];
      case '4':[[fallthrough]];
      case '5':[[fallthrough]];
      case '6':[[fallthrough]];
      case '7':
         s.push_back(decode_octal_sequence(ii));
         break;
         
      case 'x':
         // {HEXD}+
         if(std::next(ii) == end) 
            return ii; // ERROR!
         ++ii;
         if(!std::isxdigit(*ii))
            return ii; // ERROR
         s.push_back(decode_hexidecimal_sequence(ii));
         break;

      // We don't do unicode (to UTF8??)
         
      default:
         return ii; // ERROR!
      }
      
   }

   return end; 
}

} // namespace giraffe
