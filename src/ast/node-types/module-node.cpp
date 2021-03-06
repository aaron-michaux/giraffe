#include "stdinc.hpp"

#include "module-node.hpp"

#define This ModuleNode

namespace giraffe
{

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   // export import module identifier
   for(auto i = 0; i < indent; ++i) ss << ' ';
   if(is_export()) ss << "export ";
   if(is_import()) ss << "import ";
   if(is_module()) {
      ss << "module";
      if(text().size() > 0) ss << ' ';
   }

   if(is_local_include()) {
      ss << '"' << text() << '"';
   } else if(is_system_include()) {
      ss << '<' << text() << '>';
   } else {
      ss << text();
   }

   ss << ";\n";
   return ss;
}

} // namespace giraffe
#undef This
