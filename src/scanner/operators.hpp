
#pragma once

#include "token-types.h"

namespace giraffe
{
constexpr bool is_unary_postfix_op(uint16_t token_id) noexcept;
constexpr bool is_unary_prefix_op(uint16_t token_id) noexcept;
constexpr bool is_unary_op(uint16_t token_id) noexcept;
constexpr bool is_binary_op(uint16_t token_id) noexcept;
constexpr bool is_ternary_op(uint16_t token_id) noexcept;

constexpr bool is_left_right_associative(uint16_t token_id) noexcept;

constexpr int unary_postfix_op_precedence(uint16_t token_id) noexcept;
constexpr int unary_prefix_op_precedence(uint16_t token_id) noexcept;
constexpr int binary_op_precedence(uint16_t token_id) noexcept;
constexpr int operator_precedence_q(uint16_t token_id) noexcept;

constexpr bool is_keyword(uint16_t token_id) noexcept;
constexpr bool is_str_part(uint16_t token_id) noexcept;

// ------------------------------------------------------------- implementations

constexpr bool is_unary_postfix_op(uint16_t token_id) noexcept
{
   switch(token_id) {
   }
   return false;
}

constexpr bool is_unary_prefix_op(uint16_t token_id) noexcept
{
   switch(token_id) {
   case TMINUS:
   case TSHOUT: return true;
   }
   return false;
}

constexpr bool is_unary_op(uint16_t token_id) noexcept
{
   return is_unary_postfix_op(token_id) || is_unary_prefix_op(token_id);
}

constexpr bool is_binary_op(uint16_t token_id) noexcept
{
   return binary_op_precedence(token_id) > 0;
}

constexpr bool is_ternary_op(uint16_t token_id) noexcept
{
   switch(token_id) {
   }
   return false;
}

constexpr bool is_left_right_associative(uint16_t token_id) noexcept
{
   switch(token_id) {
      /* left-to-right */
   case TANDAND:   //
   case TOROR:     //
   case TEQEQ:     //
      
   case TNE:       //
   case TLT:       //
   case TLE:       //
   case TGT:       //
   case TGE:       //
   case TMULT:     //
   case TDIV:      //
   case TPLUS:     //
   case TMINUS:    //
   case TLPAREN:   //
      return true;

   /* right-to-left */
      return false;
   }

   if(is_binary_op(token_id) || is_ternary_op(token_id)) assert(false);

   return false;
}

constexpr int unary_postfix_op_precedence(uint16_t token_id) noexcept
{
   switch(token_id) {
   }
   return 0;
}

constexpr int unary_prefix_op_precedence(uint16_t token_id) noexcept
{
   // Everything has the same precedence
   switch(token_id) {
   case TPLUS:
   case TMINUS:
   case TSHOUT: return 23;
   }
   return 0;
}

constexpr int binary_op_precedence(uint16_t token_id) noexcept
{
   switch(token_id) {
   case TLPAREN: return 30;   //
   case TMULT:                //   
   case TDIV:                 //   
   case TREMAINDER: return 25;//   * / %
   case TPLUS:                //
   case TMINUS: return 24;    //   +  -
   case TLT:                  //
   case TLE:                  //
   case TGT:                  //
   case TGE: return 17;       //  > >=  < <=
   case TEQEQ:                //
   case TNE: return 16;       //  == !=
   case TANDAND: return 2;    //  &&
   case TOROR: return 1;      //  ||
   }
   return 0;
}

constexpr int operator_precedence_q(uint16_t token_id) noexcept
{
   const auto is_binary     = is_binary_op(token_id);
   const auto op_precedence = is_binary ? binary_op_precedence(token_id)
                                        : unary_postfix_op_precedence(token_id);
   const auto assoc_diff    = is_left_right_associative(token_id) ? 1 : 0;
   return op_precedence + (is_binary ? assoc_diff : 0);
}

constexpr bool is_keyword(uint16_t token_id) noexcept
{
   return token_id >= TIF && token_id <= TUNDEF;
}

} // namespace giraffe
