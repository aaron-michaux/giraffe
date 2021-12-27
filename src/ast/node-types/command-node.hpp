
#pragma once

#include "ast/node-types/ast-node.hpp"

// @see https://docs.microsoft.com/en-us/cpp/preprocessor/hash-define-directive-c-cpp?view=msvc-170

namespace giraffe
{
enum class CommandType : uint8_t {
   NONE = 0,
   INCLUDE,
   DEFINE,
   UNDEF,
   ERROR,
   PRAGMA
};

/**
 */
class CommandNode final : public AstNode
{
private:
   CommandType type_ = CommandType::NONE;
   string text_ = {}; // 
   uint32_t line_no_ = 0; // for #line <xyz>
   bool is_local_include_ = false; // #include "file" versus <file>
   
public:
   CommandNode()
      : AstNode(NodeType::COMMAND)
   {}
   virtual ~CommandNode() = default;

   static CommandNode * make_none() noexcept;
   static CommandNode * make_include(String filename) noexcept;
   static CommandNode * make_undef(String identifier) noexcept;

   
   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto type() const noexcept { return type_; }
   //@}
};
}
