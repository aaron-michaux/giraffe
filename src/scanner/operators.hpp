
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
   switch(token_id) {}
   return false;
}

constexpr bool is_unary_prefix_op(uint16_t token_id) noexcept
{
   switch(token_id) {
   case TPLUS: [[fallthrough]];
   case TMINUS: [[fallthrough]];
   case TSHOUT: [[fallthrough]];
   case TTILDE: [[fallthrough]];
   case TDEFINED: return true;
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
   case TQUESTION: return true;
   }
   return false;
}

constexpr bool is_left_right_associative(uint16_t token_id) noexcept
{
   switch(token_id) {
   /* left-to-right */
   case TCOMMA: [[fallthrough]]; //

   case TLE: [[fallthrough]];   //
   case TGE: [[fallthrough]];   //
   case TLT: [[fallthrough]];   //
   case TGT: [[fallthrough]];   //
   case TEQEQ: [[fallthrough]]; //
   case TNE: [[fallthrough]];   //

   case TANDAND: [[fallthrough]]; //
   case TOROR: [[fallthrough]];   //

   case TPLUS: [[fallthrough]];      //
   case TMINUS: [[fallthrough]];     //
   case TMULT: [[fallthrough]];      //
   case TDIV: [[fallthrough]];       //
   case TREMAINDER: [[fallthrough]]; //

   case TAND: [[fallthrough]];
   case TOR: [[fallthrough]];
   case TCARROT: [[fallthrough]];
   case TLTLT: [[fallthrough]];
   case TGTGT: return true;

   /* right-to-left */
   case TQUESTION: [[fallthrough]];
   case TCOLON: return false;
   }

   if(is_binary_op(token_id) || is_ternary_op(token_id)) assert(false);

   return false;
}

constexpr int unary_postfix_op_precedence(uint16_t token_id) noexcept
{
   switch(token_id) {}
   return 0;
}

constexpr int unary_prefix_op_precedence(uint16_t token_id) noexcept
{
   // Everything has the same precedence
   switch(token_id) {
   case TPLUS: [[fallthrough]];
   case TMINUS: [[fallthrough]];
   case TSHOUT: [[fallthrough]];
   case TTILDE: [[fallthrough]];
   case TDEFINED: return 23;
   }
   return 0;
}

constexpr int binary_op_precedence(uint16_t token_id) noexcept
{
   // @see https://en.cppreference.com/w/cpp/language/operator_precedence
   switch(token_id) {
   case TCOMMA: return 20 - 17;     //  ,
   case TQUESTION: [[fallthrough]]; //
   case TCOLON: return 20 - 16;     //  ?  :
   case TOROR: return 20 - 15;      //  ||
   case TANDAND: return 20 - 14;    //  &&
   case TOR: return 20 - 13;        //  |
   case TCARROT: return 20 - 12;    //  ^
   case TAND: return 20 - 11;       //  &
   case TEQEQ: [[fallthrough]];     //
   case TNE: return 20 - 10;        //  == !=
   case TLT: [[fallthrough]];       //
   case TLE: [[fallthrough]];       //
   case TGT: [[fallthrough]];       //
   case TGE: return 20 - 9;         //  > >=  < <=
   case TSPACESHIP: return 20 - 8;  //  <=>
   case TLTLT: [[fallthrough]];     //
   case TGTGT: return 20 - 7;       //  <<  >>
   case TPLUS: [[fallthrough]];     //
   case TMINUS: return 20 - 6;      //  +  -
   case TMULT: [[fallthrough]];     //
   case TDIV: [[fallthrough]];      //
   case TREMAINDER: return 20 - 5;  //  * / %
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
