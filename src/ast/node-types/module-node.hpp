
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
/**
 */
class ModuleNode final : public AstNode
{
private:
   string identifier_ = ""s;
   bool is_import_ = false;
   bool is_export_ = false;
   bool is_module_ = false;

public:
   ModuleNode(bool is_import, bool is_export, bool is_module, string identifier)
      : AstNode(NodeType::MODULE)
      , identifier_(std::move(identifier))
      , is_import_(is_import)
      , is_export_(is_export)
      , is_module_(is_module)
   {}
   virtual ~ModuleNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   bool is_import() const noexcept { return is_import_; }
   bool is_export() const noexcept { return is_export_; }
   bool is_module() const noexcept { return is_module_; }
   const auto& identifier() const noexcept { return identifier_; }
   //@}
};
}

