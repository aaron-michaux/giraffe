
#include "stdinc.hpp"

#include <stdexcept>

#include "eval-expression.hpp"

namespace giraffe
{
// ----------------------------------------------------------------------------------- parse integer
/**
 * This text is from cppreference.com:
 * @see https://en.cppreference.com/w/cpp/language/integer_literal
 *
 * decimal-literal: is a non-zero decimal digit [1-9], followed by zero or more decimal
 * digits [0-9]
 *
 * octal-literal: is the digit zero (0) followed by zero or more octal digits [0-7]
 *
 * hex-literal: is the character sequence 0x or the character sequence 0X followed by one
 * or more hexadecimal digits [0-9a-fA-F]
 *
 * binary-literal: is the character sequence 0b or the character sequence 0B followed by one
 * or more binary digits (0, 1)
 *
 * integer-suffix: if provided, may contain one or both of the following (if both are provided,
 * they may appear in any order:
 *    unsigned-suffix: (the character u or the character U)
 *    one of:
 *       long-suffix: (the character l or the character L)
 *       long-long-suffix: (the character sequence ll or the character sequence LL)
 *       size-suffix (the character z or the character Z)
 *
 * Optional single quotes (') may be inserted between the digits as a separator. They are
 * ignored by the compiler.
 */
Integer parse_integer(string_view text) noexcept(false) { return {}; }

// ---------------------------------------------------------------------------------- evalutate expr

Integer evaluate_expr(const ExpressionNode* expr) noexcept(false)
{
   switch(expr->expr_type()) {
   case ExprType::NONE: //
      throw std::logic_error{"attempt to evaluate a 'none' expression"};
   case ExprType::EMPTY: //
      throw std::logic_error{"attempt to evaluate an 'empty' expression"};
   case ExprType::IDENTIFIER:
      throw std::runtime_error{format("failed to resolve symbol: '{}'", expr->text())};
   case ExprType::INTEGER: //
      return parse_integer(expr->text());
   case ExprType::SUBEXPR: //
      assert(expr->size() == 1);
      return evaluate_expr(expr->child(0));
   case ExprType::UNARY:
      assert(expr->size() == 1);
      switch(expr->op()) {
      case TSHOUT: return evaluate_expr(expr->child(0)).unot();
      case TPLUS: return evaluate_expr(expr->child(0)).uplus();
      case TMINUS: return evaluate_expr(expr->child(0)).uminus();
      case TTILDE: return evaluate_expr(expr->child(0)).utilde();
      }
      throw std::logic_error{
          format("unexpected unary operator: '{}'", token_id_to_str(expr->op()))};
   case ExprType::BINARY:
      assert(expr->size() == 2);
      switch(expr->op()) {
      case TPLUS: return evaluate_expr(expr->lhs()).plus(evaluate_expr(expr->rhs()));
      case TMINUS: return evaluate_expr(expr->lhs()).minus(evaluate_expr(expr->rhs()));
      case TMULT: return evaluate_expr(expr->lhs()).multiply(evaluate_expr(expr->rhs()));
      case TDIV: return evaluate_expr(expr->lhs()).divide(evaluate_expr(expr->rhs()));
      case TREMAINDER: return evaluate_expr(expr->lhs()).remainder(evaluate_expr(expr->rhs()));
      case TLTLT: return evaluate_expr(expr->lhs()).left_shift(evaluate_expr(expr->rhs()));
      case TGTGT: return evaluate_expr(expr->lhs()).right_shift(evaluate_expr(expr->rhs()));
      case TLT: return evaluate_expr(expr->lhs()).less(evaluate_expr(expr->rhs()));
      case TLE: return evaluate_expr(expr->lhs()).less_eq(evaluate_expr(expr->rhs()));
      case TGT: return evaluate_expr(expr->lhs()).greater(evaluate_expr(expr->rhs()));
      case TGE: return evaluate_expr(expr->lhs()).greater_eq(evaluate_expr(expr->rhs()));
      case TEQEQ: return evaluate_expr(expr->lhs()).equal(evaluate_expr(expr->rhs()));
      case TNE: return evaluate_expr(expr->lhs()).not_equal(evaluate_expr(expr->rhs()));
      case TAND: return evaluate_expr(expr->lhs()).bitwise_and(evaluate_expr(expr->rhs()));
      case TCARROT: return evaluate_expr(expr->lhs()).bitwise_xor(evaluate_expr(expr->rhs()));
      case TOR: return evaluate_expr(expr->lhs()).bitwise_or(evaluate_expr(expr->rhs()));
      case TANDAND: return evaluate_expr(expr->lhs()).logical_and(evaluate_expr(expr->rhs()));
      case TOROR: return evaluate_expr(expr->lhs()).logical_or(evaluate_expr(expr->rhs()));
      }
      throw std::logic_error{
          format("unexpected binary operator: '{}'", token_id_to_str(expr->op()))};
   }
   assert(false); // logic error
   return {};
}

} // namespace giraffe
