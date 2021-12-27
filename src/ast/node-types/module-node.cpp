#include "stdinc.hpp"
#include "module-node.hpp"

#define This ModuleNode

namespace giraffe {

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   // export import module identifier
   for(auto i = 0; i < indent; ++i) ss << ' ';
   if(is_export()) ss << "export ";
   if(is_import()) ss << "import ";
   if(is_module()) ss << "module ";
   ss << identifier();
   ss << '\n';
   return ss;
}

}
