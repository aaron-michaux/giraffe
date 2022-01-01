
#pragma once

#include "ast/node-types/ast-node.hpp"

namespace giraffe
{
enum class ExprType : uint8_t {
   NONE = 0,
   EMPTY,
   IDENTIFIER,
   INTEGER,
   SUBEXPR,
   UNARY,
   BINARY
};

/**
 */
class ExpressionNode final : public AstNode
{
 private:
   sso23::string text_    = {};
   int op_                = TNONE; // The operator (binary/unary only)
   SourceLocation op_loc_ = {};    // Location of the operator (binary/unary only)
   ExprType expr_type_    = ExprType::NONE;

   ExpressionNode(ExprType expr_type, SourceRange expr_range = {})
       : AstNode(NodeType::EXPRESSION, expr_range)
       , expr_type_(expr_type)
   {}

 public:
   virtual ~ExpressionNode() = default;

   static ExpressionNode* make_empty() noexcept;
   static ExpressionNode* make_identifier(const SourceRange expr_range,
                                          string_view identifier) noexcept;
   static ExpressionNode* make_integer(const SourceRange expr_range,
                                       string_view integer_str) noexcept;
   static ExpressionNode* make_subexpr(const SourceRange expr_range,
                                       ExpressionNode* subexpr) noexcept;
   static ExpressionNode* make_unary(const int op,
                                     const SourceLocation op_loc,
                                     const SourceRange expr_range,
                                     ExpressionNode* expr) noexcept;
   static ExpressionNode* make_binary(const int op,
                                      const SourceLocation op_loc,
                                      const SourceRange expr_range,
                                      ExpressionNode* lhs,
                                      ExpressionNode* rhs) noexcept;

   std::ostream& stream(std::ostream& ss, const int indent) const noexcept override;

   //@{ Getters
   auto expr_type() const noexcept { return expr_type_; }
   auto op() const noexcept { return op_; }
   auto op_loc() const noexcept { return op_loc_; }
   auto text() const noexcept { return string_view{text_.data(), text_.size()}; }

   auto child(size_t index) noexcept { return cast_child_<ExpressionNode>(index); }
   auto lhs() noexcept { return cast_child_<ExpressionNode>(0); }
   auto rhs() noexcept { return cast_child_<ExpressionNode>(1); }
   auto child(size_t index) const noexcept { return cast_child_<ExpressionNode>(index); }
   auto lhs() const noexcept { return cast_child_<ExpressionNode>(0); }
   auto rhs() const noexcept { return cast_child_<ExpressionNode>(1); }
   //@}
};
} // namespace giraffe
