
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
   LINE,
   ERROR,
   PRAGMA
};

/**
 */
class CommandNode final : public AstNode
{
private:
   CommandType type_ = CommandType::NONE;

public:
   CommandNode()
      : AstNode(NodeType::COMMAND)
   {}
   virtual ~CommandNode() = default;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto type() const noexcept { return type_; }
   //@}
};
}
