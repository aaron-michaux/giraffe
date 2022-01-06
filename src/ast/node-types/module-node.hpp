
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 */
class ModuleNode final : public AstNode
{
 private:
   sso23::string text_     = {}; // the include name
   bool is_import_         = false;
   bool is_export_         = false;
   bool is_module_         = false;
   bool is_local_include_  = false; // TRUE iff: import "something.hpp"
   bool is_system_include_ = false; // TRUF iff: import <something.hpp>

 public:
   ModuleNode(SourceRange loc,
              bool is_import,
              bool is_export,
              bool is_module,
              bool is_local_include,
              bool is_system_include,
              string_view text)
       : AstNode(NodeType::MODULE, loc)
       , text_(text)
       , is_import_(is_import)
       , is_export_(is_export)
       , is_module_(is_module)
       , is_local_include_(is_local_include)
       , is_system_include_(is_system_include)
   {}
   virtual ~ModuleNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   bool is_import() const noexcept { return is_import_; }
   bool is_export() const noexcept { return is_export_; }
   bool is_module() const noexcept { return is_module_; }
   bool is_include() const noexcept { return is_system_include() || is_local_include(); }
   bool is_local_include() const noexcept { return is_local_include_; }
   bool is_system_include() const noexcept { return is_system_include_; }
   auto text() const noexcept { return string_view{text_.data(), text_.size()}; }
   //@}
};
} // namespace giraffe
