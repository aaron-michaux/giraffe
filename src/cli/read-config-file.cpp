#include "stdinc.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "read-config-file.hpp"

#define This ConfigFileData

namespace giraffe
{
namespace detail
{
   static void parse_define_(ConfigFileData& data, const std::string& line) noexcept(false)
   {
      std::istringstream iss(line);
      sso23::string define, symbol, value;
      iss >> define >> symbol >> value;
      if(define != "#define") throw std::logic_error("expected '#define' at start of line");
      data.defines.emplace_back(std::move(symbol), std::move(value));
   }

   static void parse_include_(ConfigFileData& data, const std::string& line) noexcept(false)
   {
      std::istringstream iss(line);
      std::string path;
      iss >> path;
      data.include_paths.emplace_back(std::move(path), true);
   }

} // namespace detail

ConfigFileData This::read(string_view filename) noexcept(false)
{
   ConfigFileData ret = {};

   // Open up the input file
   std::ifstream infile;
   infile.open(std::string(filename));
   if(!infile.is_open()) throw std::runtime_error(format("failed to open file", filename));

   // Parse each line
   bool in_search_list = false;
   std::string line    = {};
   while(std::getline(infile, line)) {
      if(in_search_list) {
         if(line.starts_with("#include ")) {
            // ignore
         } else if(line.starts_with("End of search list")) {
            in_search_list = false;
         } else {
            detail::parse_include_(ret, line);
         }
      } else {
         if(line.starts_with("#define ")) { detail::parse_define_(ret, line); }
         if(line.starts_with("#include ")) in_search_list = true;
      }
   }

   return ret;
}

} // namespace giraffe

#undef ConfigFileData
