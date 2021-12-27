#include "stdinc.hpp"

namespace giraffe {

// ------------------------------------------------------------ precedence-climb

ExpressionNode * precedence_climb(Context& context,
                                  const int min_precedence) noexcept
{
   assert(min_precedence >= 0);
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_expressions));

   auto calc_precedence_pair = [&] () -> std::pair<int, bool> {
      const auto id = scanner.current().id();
      // Binary includes '?'
      if(is_binary_op(id))
         return {binary_op_precedence(id), is_left_right_associative(id)};
      return {-1, false};
   };

   auto next_token_is_useful = [&] () {
      return is_binary_op(scanner.current().id());
   };
   
   // Get the LHS expression
   ExpressionNode * expr = accept_unary_expr(context);
   assert(expr != nullptr);   
   
   while(next_token_is_useful()) {
      const auto [precedence, is_left_assoc] = calc_precedence_pair();
      if(precedence < min_precedence) break; // we're done
      const int next_min_precendence = precedence + (is_left_assoc ? 1 : 0);
      ExpressionNode * rhs           = nullptr;
      const auto& op                 = scanner.consume(); // the binary operator
      if(!expect(scanner, first_set_expressions)) {
         context.push_error("expected expression after operator!");
         rhs = ExpressionNode::make_none_expr();
         skip_to_sequence(scanner, TNEWLINE);    // Skip to newline
      } else {
         rhs = precedence_climb(context, next_min_precedence);
      }
      assert(rhs != nullptr);
      expr = ExpressionNode::make_binary(op.id(),
                                         op.location(),
                                         SourceRange{expr->loc0(), rhs->loc1()},
                                         expr,
                                         rhs);
   }

   return expr;
}

// ----------------------------------------------------------- accept-unary-expr

ExpressionNode * accept_unary_expr(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_expressions));
   return nullptr;
}

// ----------------------------------------------------------- accept-expression

ExpressionNode * accept_expression(Context& context) noexcept
{
   Scanner& scanner = context.scanner();
   assert(expect(scanner, first_set_expressions));
   return precedence_climb(context, 0);
}

}
