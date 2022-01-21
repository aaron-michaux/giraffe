
#include "stdinc.hpp"

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
Integer parse_integer(string_view text) noexcept(false)
{
   // Decode the prefix
   const bool is_hex     = text.starts_with("0x") || text.starts_with("0X");
   const bool is_binary  = text.starts_with("0b") || text.starts_with("0B");
   const bool is_octal   = !is_hex && !is_binary && text.starts_with("0");
   const bool is_decimal = !is_hex && !is_binary && !is_octal;

   // [start, finish) of the number
   const auto start  = (is_hex || is_binary) ? (begin(text) + 2) : begin(text);
   const auto finish = std::find_if_not(start, end(text), [](char ch) -> bool {
      return std::isxdigit(ch) || ch == '\''; // [0-9a-fA-F']
   });
   const auto len    = std::distance(start, finish);

   // Get the suffix
   auto count_ch = [finish, &text](char ref) -> int {
      int counter = 0;
      std::for_each(finish, end(text), [&](char ch) {
         if(std::tolower(ch) == ref) ++counter;
      });
      return counter;
   };

   const bool is_unsigned  = count_ch('u') > 0;  // u or U
   const bool is_size      = count_ch('z') > 0;  // z or Z
   const bool is_long      = count_ch('l') == 1; // l or L
   const bool is_long_long = count_ch('l') > 1;  // ll or LL
   const bool is_int       = !is_long && !is_long_long && !is_size;

   // Copy text into the buffer, to ensure that the null-byte is there, and remove '
   std::array<char, 100> buffer;
   if(len + 1 >= buffer.size())
      throw std::runtime_error(format("integer overflow parsing '{}'", text));
   const auto buffer_end_iterator
       = std::copy_if(start, finish, begin(buffer), [](char ch) { return std::isxdigit(ch); });
   const auto buffer_len = std::distance(begin(buffer), buffer_end_iterator);
   buffer.at(buffer_len) = '\0';

   // Parse the integer
   char* end       = nullptr;
   const int base  = (is_hex ? 16 : is_binary ? 2 : is_octal ? 8 : 10);
   uint64_t uvalue = 0;
   int64_t value   = 0;
   if(is_unsigned)
      uvalue = strtoull(buffer.data(), &end, base);
   else
      value = strtoll(buffer.data(), &end, base);
   if(end != buffer.data() + buffer_len)
      throw std::runtime_error(format("error parsing '{}'", text));

   // Look for integer overflows...
   const int64_t min_value = //
       (is_unsigned)    ? 0
       : (is_int)       ? std::numeric_limits<int>::lowest()
       : (is_long)      ? std::numeric_limits<long>::lowest()
       : (is_long_long) ? std::numeric_limits<long long>::lowest()
       : (is_size)      ? std::numeric_limits<ssize_t>::lowest()
                        : 0;
   const uint64_t max_value = //
       (!is_unsigned && is_int)         ? std::numeric_limits<int>::max()
       : (!is_unsigned && is_long)      ? std::numeric_limits<long>::max()
       : (!is_unsigned && is_long_long) ? std::numeric_limits<long long>::max()
       : (!is_unsigned && is_size)     ? std::numeric_limits<std::make_signed_t<std::size_t>>::max()
       : (is_unsigned && is_int)       ? std::numeric_limits<unsigned int>::max()
       : (is_unsigned && is_long)      ? std::numeric_limits<unsigned long>::max()
       : (is_unsigned && is_long_long) ? std::numeric_limits<unsigned long long>::max()
       : (is_unsigned && is_size)      ? std::numeric_limits<size_t>::max()
                                       : 0;
   if(is_unsigned) {
      if(uvalue > max_value)
         throw std::runtime_error(format("integer overflow parsing '{}'", text));
   } else {
      if((value > 0) && uint64_t(value) > max_value)
         throw std::runtime_error(format("integer overflow parsing '{}'", text));
      if(value < min_value) throw std::runtime_error(format("integer overflow parsing '{}'", text));
   }

   // Return the correct type
   return (!is_unsigned && is_int)         ? Integer{static_cast<int>(value)}
          : (!is_unsigned && is_long)      ? Integer{static_cast<long>(value)}
          : (!is_unsigned && is_long_long) ? Integer{static_cast<long long>(value)}
          : (!is_unsigned && is_size)      ? Integer{static_cast<ssize_t>(value)}
          : (is_unsigned && is_int)        ? Integer{static_cast<unsigned int>(uvalue)}
          : (is_unsigned && is_long)       ? Integer{static_cast<unsigned long>(uvalue)}
          : (is_unsigned && is_long_long)  ? Integer{static_cast<unsigned long long>(uvalue)}
          : (is_unsigned && is_size)       ? Integer{static_cast<size_t>(uvalue)}
                                           : Integer{};
}

// ---------------------------------------------------------------------------------- evalutate expr

Integer evaluate_expr(Context& context,
                      const SymbolTable& symbols,
                      const ExpressionNode* expr) noexcept(false)
{
   auto eval_symbol = [&context, &symbols, expr]() -> Integer {
      if(!symbols.has(expr->text())) {
         context.push_error(expr->loc0(), expr->src_range(), "failed to resolve symbol");
         return Integer::make_invalid();
      }

      FATAL(format("we have to ask context to parse again"));
      // const auto resolved = symbols.eval(expr->text());
      const std::string resolved = "123";

      try {
         return parse_integer(resolved);
      } catch(std::exception& e) {
         context.push_error(
             expr->loc0(),
             expr->src_range(),
             format("failed to parse symbol (which evaluated to {}): {}", resolved, e.what()));
      }
      return Integer::make_invalid();
   };

   auto eval_unary = [&context, &symbols, expr]() -> Integer {
      assert(expr->size() == 1);
      const Integer child = evaluate_expr(context, symbols, expr->child(0));
      if(child.is_invalid()) return child;
      switch(expr->op()) {
      case TSHOUT: return child.unot();
      case TPLUS: return child.uplus();
      case TMINUS: return child.uminus();
      case TTILDE: return child.utilde();
      }
      context.push_error(expr->op_loc(),
                         format("unexpected unary operator: '{}'", token_id_to_str(expr->op())));
      return Integer::make_invalid();
   };

   auto eval_binary = [&context, &symbols, expr]() -> Integer {
      assert(expr->size() == 2);
      const auto lhs = evaluate_expr(context, symbols, expr->lhs());
      const auto rhs = evaluate_expr(context, symbols, expr->rhs());
      if(lhs.is_invalid() || rhs.is_invalid()) return Integer::make_invalid();

      switch(expr->op()) {
      case TPLUS: return lhs.plus(rhs);
      case TMINUS: return lhs.minus(rhs);
      case TMULT: return lhs.multiply(rhs);
      case TDIV: return lhs.divide(rhs);
      case TREMAINDER: return lhs.remainder(rhs);
      case TLTLT: return lhs.left_shift(rhs);
      case TGTGT: return lhs.right_shift(rhs);
      case TLT: return lhs.less(rhs);
      case TLE: return lhs.less_eq(rhs);
      case TGT: return lhs.greater(rhs);
      case TGE: return lhs.greater_eq(rhs);
      case TEQEQ: return lhs.equal(rhs);
      case TNE: return lhs.not_equal(rhs);
      case TAND: return lhs.bitwise_and(rhs);
      case TCARROT: return lhs.bitwise_xor(rhs);
      case TOR: return lhs.bitwise_or(rhs);
      case TANDAND: return lhs.logical_and(rhs);
      case TOROR: return lhs.logical_or(rhs);
      }
      context.push_error(expr->op_loc(),
                         format("unexpected binary operator: '{}'", token_id_to_str(expr->op())));
      return Integer::make_invalid();
   };

   switch(expr->expr_type()) {
   case ExprType::NONE: throw std::logic_error{"attempt to evaluate a 'none' expression"};
   case ExprType::EMPTY: throw std::logic_error{"attempt to evaluate an 'empty' expression"};
   case ExprType::IDENTIFIER: return eval_symbol();
   case ExprType::INTEGER: return parse_integer(expr->text());
   case ExprType::SUBEXPR:
      assert(expr->size() == 1);
      return evaluate_expr(context, symbols, expr->child(0));
   case ExprType::UNARY: return eval_unary();
   case ExprType::BINARY: return eval_binary();
   }
   assert(false); // logic error
   return {};
}

} // namespace giraffe
