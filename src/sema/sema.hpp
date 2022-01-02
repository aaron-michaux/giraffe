
#pragma once

#include "ast/ast.hpp"

namespace giraffe
{

enum class IntegerType : int {
   INT,
   LONG,
   LONGLONG,
   SIZE_T,
   UINT,
   ULONG,
   ULONGLONG,
   SSIZE_T
};

struct Integer
{
 private:
   union Data
   {
      int i;
      long l;
      long long ll;
      std::size_t sz;
      unsigned int ui;
      unsigned long ul;
      unsigned long long ull;
      std::make_signed<std::size_t> usz;
   } data_;

   IntegerType type_ = IntegerType::INT;

 public:
   Integer() { data_.i = 0; }
   Integer(const Integer&) = default;
   Integer(Integer&&)      = default;
   ~Integer()              = default;
   Integer& operator=(const Integer&) = default;
   Integer& operator=(Integer&&) = default;
};

/**
 * All expressions evaluate to an integer type of some form
 */
Integer evaluate_expr(const ExpressionNode* expr) noexcept(false);

} // namespace giraffe
