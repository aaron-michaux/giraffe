
#include "stdinc.hpp"

#include "integer.hpp"

#define This Integer

namespace giraffe::detail
{
constexpr std::size_t int_size_(IntegerType type) noexcept
{
   switch(type) {
   case IntegerType::CHAR: return sizeof(char);
   case IntegerType::SHORT: return sizeof(short);
   case IntegerType::INT: return sizeof(int);
   case IntegerType::LONG: return sizeof(long);
   case IntegerType::LONGLONG: return sizeof(long long);
   case IntegerType::UCHAR: return sizeof(unsigned char);
   case IntegerType::USHORT: return sizeof(unsigned short);
   case IntegerType::UINT: return sizeof(unsigned int);
   case IntegerType::ULONG: return sizeof(unsigned long);
   case IntegerType::ULONGLONG: return sizeof(unsigned long long);
   }
}

template<typename T> constexpr T sc_(const Integer& i) noexcept
{
   switch(i.type()) {
   case IntegerType::CHAR: return static_cast<T>(i.data().c);
   case IntegerType::SHORT: return static_cast<T>(i.data().s);
   case IntegerType::INT: return static_cast<T>(i.data().i);
   case IntegerType::LONG: return static_cast<T>(i.data().l);
   case IntegerType::LONGLONG: return static_cast<T>(i.data().ll);
   case IntegerType::UCHAR: return static_cast<T>(i.data().uc);
   case IntegerType::USHORT: return static_cast<T>(i.data().us);
   case IntegerType::UINT: return static_cast<T>(i.data().ui);
   case IntegerType::ULONG: return static_cast<T>(i.data().ul);
   case IntegerType::ULONGLONG: return static_cast<T>(i.data().ull);
   }
   assert(false); // Logic error
   return T{0};
}

constexpr bool is_signed_(IntegerType type) noexcept
{
   switch(type) {
   case IntegerType::CHAR: return std::is_signed<char>::value;
   case IntegerType::SHORT: [[fallthrough]];
   case IntegerType::INT: [[fallthrough]];
   case IntegerType::LONG: [[fallthrough]];
   case IntegerType::LONGLONG: return true;
   case IntegerType::UCHAR: [[fallthrough]];
   case IntegerType::USHORT: [[fallthrough]];
   case IntegerType::UINT: [[fallthrough]];
   case IntegerType::ULONG: [[fallthrough]];
   case IntegerType::ULONGLONG: return false;
   }
   assert(false); // This is a logic error
   return false;
}

#define UNARY_OP_(func, op)                                             \
   template<typename T> static constexpr Integer func(T value) noexcept \
   {                                                                    \
      return Integer{op value};                                         \
   }

UNARY_OP_(unot_, !)
UNARY_OP_(uplus_, +)
UNARY_OP_(uminus_, -)
UNARY_OP_(utilde_, ~)
#undef UNARY_OP_

#define BINARY_OP_(func, op)                                                         \
   template<typename U, typename V> static constexpr Integer func(U u, V v) noexcept \
   {                                                                                 \
      return Integer{u op v};                                                        \
   }

BINARY_OP_(plus_, +)
BINARY_OP_(minus_, -)
BINARY_OP_(multiply_, *)
BINARY_OP_(divide_, /)
BINARY_OP_(remainder_, %)
BINARY_OP_(left_shift_, <<)
BINARY_OP_(right_shift_, >>)
BINARY_OP_(less_, <)
BINARY_OP_(less_eq_, <=)
BINARY_OP_(greater_, >)
BINARY_OP_(greater_eq_, >=)
BINARY_OP_(equal_, ==)
BINARY_OP_(not_equal_, !=)
BINARY_OP_(bitwise_and_, &)
BINARY_OP_(bitwise_xor_, ^)
BINARY_OP_(bitwise_or_, |)
BINARY_OP_(logical_and_, &&)
BINARY_OP_(logical_or_, ||)

#undef BINARY_OP_

} // namespace giraffe::detail

namespace giraffe
{

Integer This::make_invalid() noexcept
{
   Integer ret{0};
   ret.is_valid_ = false;
   return ret;
}

bool This::is_signed() const noexcept { return detail::is_signed_(type()); }
int64_t This::signed_value() const noexcept { return detail::sc_<int64_t>(*this); }
uint64_t This::unsigned_value() const noexcept { return detail::sc_<uint64_t>(*this); }

bool This::operator==(const Integer& rhs) const noexcept
{
   if(this->type() != rhs.type()) return false;
   return (is_signed()) ? signed_value() == rhs.signed_value()
                        : unsigned_value() == rhs.unsigned_value();
}

std::string This::to_string() const noexcept
{
   switch(type()) {
   case IntegerType::CHAR: return format("{}", detail::sc_<char>(*this));
   case IntegerType::SHORT: return format("{}", detail::sc_<short>(*this));
   case IntegerType::INT: return format("{}", detail::sc_<int>(*this));
   case IntegerType::LONG: return format("{}", detail::sc_<long>(*this));
   case IntegerType::LONGLONG: return format("{}", detail::sc_<long long>(*this));
   case IntegerType::UCHAR: return format("{}", detail::sc_<unsigned char>(*this));
   case IntegerType::USHORT: return format("{}", detail::sc_<unsigned short>(*this));
   case IntegerType::UINT: return format("{}", detail::sc_<unsigned int>(*this));
   case IntegerType::ULONG: return format("{}", detail::sc_<unsigned long>(*this));
   case IntegerType::ULONGLONG: return format("{}", detail::sc_<unsigned long long>(*this));
   }
   assert(false); // logic error
   return ""s;
}

#define UNARY_OP(fname)                                                             \
   Integer This::fname() const noexcept                                             \
   {                                                                                \
      using namespace detail;                                                       \
      switch(type()) {                                                              \
      case IntegerType::CHAR: return fname##_(sc_<char>(*this));                    \
      case IntegerType::SHORT: return fname##_(sc_<short>(*this));                  \
      case IntegerType::INT: return fname##_(sc_<int>(*this));                      \
      case IntegerType::LONG: return fname##_(sc_<long>(*this));                    \
      case IntegerType::LONGLONG: return fname##_(sc_<long long>(*this));           \
      case IntegerType::UCHAR: return fname##_(sc_<unsigned char>(*this));          \
      case IntegerType::USHORT: return fname##_(sc_<unsigned short>(*this));        \
      case IntegerType::UINT: return fname##_(sc_<unsigned int>(*this));            \
      case IntegerType::ULONG: return fname##_(sc_<unsigned long>(*this));          \
      case IntegerType::ULONGLONG: return fname##_(sc_<unsigned long long>(*this)); \
      }                                                                             \
      assert(false); /* logic error */                                              \
      return Integer{0};                                                            \
   }

UNARY_OP(unot)
UNARY_OP(uplus)
UNARY_OP(uminus)
UNARY_OP(utilde)
#undef UNARY_OP

#define BINOP_INNER(OP, TYPE)                                                            \
   switch(rhs.type()) {                                                                  \
      using namespace detail;                                                            \
   case IntegerType::CHAR: return OP(sc_<TYPE>(lhs), sc_<char>(rhs));                    \
   case IntegerType::SHORT: return OP(sc_<TYPE>(lhs), sc_<short>(rhs));                  \
   case IntegerType::INT: return OP(sc_<TYPE>(lhs), sc_<int>(rhs));                      \
   case IntegerType::LONG: return OP(sc_<TYPE>(lhs), sc_<long>(rhs));                    \
   case IntegerType::LONGLONG: return OP(sc_<TYPE>(lhs), sc_<long long>(rhs));           \
   case IntegerType::UCHAR: return OP(sc_<TYPE>(lhs), sc_<unsigned char>(rhs));          \
   case IntegerType::USHORT: return OP(sc_<TYPE>(lhs), sc_<unsigned short>(rhs));        \
   case IntegerType::UINT: return OP(sc_<TYPE>(lhs), sc_<unsigned int>(rhs));            \
   case IntegerType::ULONG: return OP(sc_<TYPE>(lhs), sc_<unsigned long>(rhs));          \
   case IntegerType::ULONGLONG: return OP(sc_<TYPE>(lhs), sc_<unsigned long long>(rhs)); \
   }                                                                                     \
   assert(false); /* logic error */                                                      \
   return Integer{0};

#define BINARY_OPERATOR(fname)                                               \
   Integer This::fname(const Integer& rhs) const noexcept                    \
   {                                                                         \
      const auto& lhs = *this;                                               \
      switch(lhs.type()) {                                                   \
      case IntegerType::CHAR: BINOP_INNER(fname##_, char)                    \
      case IntegerType::SHORT: BINOP_INNER(fname##_, short)                  \
      case IntegerType::INT: BINOP_INNER(fname##_, int)                      \
      case IntegerType::LONG: BINOP_INNER(fname##_, long)                    \
      case IntegerType::LONGLONG: BINOP_INNER(fname##_, long long)           \
      case IntegerType::UCHAR: BINOP_INNER(fname##_, unsigned char)          \
      case IntegerType::USHORT: BINOP_INNER(fname##_, unsigned short)        \
      case IntegerType::UINT: BINOP_INNER(fname##_, unsigned int)            \
      case IntegerType::ULONG: BINOP_INNER(fname##_, unsigned long)          \
      case IntegerType::ULONGLONG: BINOP_INNER(fname##_, unsigned long long) \
      }                                                                      \
      assert(false); /* logic error */                                       \
      return Integer{0};                                                     \
   }

BINARY_OPERATOR(plus)
BINARY_OPERATOR(minus)
BINARY_OPERATOR(multiply)
BINARY_OPERATOR(divide)
BINARY_OPERATOR(remainder)
BINARY_OPERATOR(left_shift)
BINARY_OPERATOR(right_shift)
BINARY_OPERATOR(less)
BINARY_OPERATOR(less_eq)
BINARY_OPERATOR(greater)
BINARY_OPERATOR(greater_eq)
BINARY_OPERATOR(equal)
BINARY_OPERATOR(not_equal)
BINARY_OPERATOR(bitwise_and)
BINARY_OPERATOR(bitwise_xor)
BINARY_OPERATOR(bitwise_or)
BINARY_OPERATOR(logical_and)
BINARY_OPERATOR(logical_or)

#undef BINARY_OPERATOR
#undef BINOP_INNER

} // namespace giraffe

#undef This
