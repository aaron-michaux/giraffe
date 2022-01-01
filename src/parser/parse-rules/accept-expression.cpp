#include "stdinc.hpp"

#include "parser/parser-internal.hpp"

namespace giraffe
{

ExpressionNode* accept_nested_expr(Context& context) noexcept;
ExpressionNode* accept_unary_expr(Context& context) noexcept;
ExpressionNode* accept_primary_expr(Context& context) noexcept;

// ------------------------------------------------------------ precedence-climb

ExpressionNode* precedence_climb(Context& context, const int min_precedence) noexcept
{
   assert(min_precedence >= 0);
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_expressions));

   auto calc_precedence_pair = [&]() -> std::pair<int, bool> {
      const auto id = scanner.current().id();
      // Binary includes '?'
      if(is_binary_op(id))
         return {binary_op_precedence(id), is_left_right_associative(id)};
      return {-1, false};
   };

   auto next_token_is_useful = [&]() { return is_binary_op(scanner.current().id()); };

   // Get the LHS expression
   ExpressionNode* expr = accept_unary_expr(context);
   assert(expr != nullptr);

   while(next_token_is_useful()) {
      const auto [precedence, is_left_assoc] = calc_precedence_pair();
      if(precedence < min_precedence) break; // we're done
      const int next_min_precedence = precedence + (is_left_assoc ? 1 : 0);
      ExpressionNode* rhs           = nullptr;
      const auto& op                = scanner.consume(); // the binary operator
      if(!expect(scanner, first_set_expressions)) {
         context.push_error("expected expression after operator!");
         rhs = ExpressionNode::make_empty();
         skip_to_sequence(scanner, TNEWLINE); // Skip to newline
      } else {
         rhs = precedence_climb(context, next_min_precedence);
      }
      assert(rhs != nullptr);
      expr = ExpressionNode::make_binary(
          op.id(), op.location(), SourceRange{expr->loc0(), rhs->loc1()}, expr, rhs);
   }

   return expr;
}

// ---------------------------------------------------------- accept-nested-expr

ExpressionNode* accept_nested_expr(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, TLPAREN));
   const auto loc0 = scanner.consume().location();

   if(!expect(scanner, first_set_expressions)) {
      context.push_error("expected an expression");
      skip_to_sequence(scanner, TNEWLINE); // Skip to newline
      return ExpressionNode::make_empty();
   }

   auto expr       = accept_expression(context);
   const auto loc1 = scanner.current().source_range().second;
   if(scanner.current().id() == TRPAREN) {
      scanner.consume(); // All is good
      expr = ExpressionNode::make_subexpr({loc0, loc1}, expr);
   } else {
      context.push_error("expected closing parenthesis");
      skip_to_sequence(scanner, TNEWLINE); // Skip to newline
   }
   return expr;
}

// --------------------------------------------------------- accept-primary-expr

ExpressionNode* accept_primary_expr(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_primary_expr));

   const auto& token = scanner.consume();
   const auto text   = token.text();
   if(token.id() == TIDENTIFIER) {
      return ExpressionNode::make_identifier(token.source_range(),
                                             {text.begin(), text.end()});
   } else if(token.id() == TINTEGER) {
      return ExpressionNode::make_integer(token.source_range(),
                                          {text.begin(), text.end()});
   } else if(token.id() == TLPAREN) {
      scanner.set_position(scanner.position() - 1);
      return accept_nested_expr(context);

   } else {
      FATAL("logic error");
   }

   // Unreachable
   return ExpressionNode::make_empty();
}

// ----------------------------------------------------------- accept-unary-expr

ExpressionNode* accept_unary_expr(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_expressions));

   if(expect(scanner, first_set_unary_op)) {
      const auto& token = scanner.consume();
      if(!expect(scanner, first_set_expressions)) {
         context.push_error("expected unary or primary expression");
         skip_to_sequence(scanner, TNEWLINE); // Skip to newline
         return ExpressionNode::make_empty();
      }

      auto expr = accept_unary_expr(context);
      const SourceRange expr_range{token.location(), expr->loc1()};
      return ExpressionNode::make_unary(token.id(), token.location(), expr_range, expr);
   }

   return accept_primary_expr(context);
}

// ----------------------------------------------------------- accept-expression

ExpressionNode* accept_expression(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_expressions));
   return precedence_climb(context, 0);
}

} // namespace giraffe
