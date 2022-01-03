
#pragma once

namespace giraffe
{
/** The different integer types */
enum class IntegerType : int {
   CHAR, // Always signed
   SHORT,
   INT,
   LONG,
   LONGLONG,
   UCHAR,
   USHORT,
   UINT,
   ULONG,
   ULONGLONG
};

} // namespace giraffe

namespace giraffe::detail
{
// -- The size of a given IntegerType (calculated at runtime)
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

} // namespace giraffe::detail

namespace giraffe
{

struct Integer
{
 private:
   union Data
   {
      char c;
      short s;
      int i;
      long l;
      long long ll;
      unsigned char uc;
      unsigned short us;
      unsigned int ui;
      unsigned long ul;
      unsigned long long ull;
   } data_;

   IntegerType type_ = IntegerType::INT;

 public:
   constexpr Integer() noexcept
       : Integer(0)
   {}

#define EXPLICIT_CONST(T, member, type)         \
   constexpr explicit Integer(T value) noexcept \
       : data_{.member = {value}}               \
       , type_{IntegerType::type}               \
   {}
   EXPLICIT_CONST(char, c, CHAR)
   EXPLICIT_CONST(short, s, SHORT)
   EXPLICIT_CONST(int, i, INT)
   EXPLICIT_CONST(long, l, LONG)
   EXPLICIT_CONST(long long, ll, LONGLONG)
   EXPLICIT_CONST(unsigned char, uc, UCHAR)
   EXPLICIT_CONST(unsigned short, us, USHORT)
   EXPLICIT_CONST(unsigned int, ui, UINT)
   EXPLICIT_CONST(unsigned long, ul, ULONG)
   EXPLICIT_CONST(unsigned long long, ull, ULONGLONG)
#undef EXPLICIT_CONST

   constexpr Integer(const Integer&) = default;
   constexpr Integer(Integer&&)      = default;
   constexpr ~Integer()              = default;
   constexpr Integer& operator=(const Integer&) = default;
   constexpr Integer& operator=(Integer&&) = default;

   constexpr auto type() const noexcept { return type_; }

   constexpr bool is_signed() const noexcept { return detail::is_signed_(type()); }
   constexpr int64_t signed_value() const noexcept { return static_cast_to_<int64_t>(); }
   constexpr uint64_t unsigned_value() const noexcept { return static_cast_to_<uint64_t>(); }

   constexpr bool operator==(const Integer& rhs) const noexcept
   {
      if(this->type() != rhs.type()) return false;
      return (is_signed()) ? signed_value() == rhs.signed_value()
                           : unsigned_value() == rhs.unsigned_value();
   }
   constexpr bool operator!=(const Integer& rhs) const noexcept { return !(*this == rhs); }

   string to_string() const noexcept
   {
      switch(type()) {
      case IntegerType::CHAR: return format("{}", static_cast_to_<char>());
      case IntegerType::SHORT: return format("{}", static_cast_to_<short>());
      case IntegerType::INT: return format("{}", static_cast_to_<int>());
      case IntegerType::LONG: return format("{}", static_cast_to_<long>());
      case IntegerType::LONGLONG: return format("{}", static_cast_to_<long long>());
      case IntegerType::UCHAR: return format("{}", static_cast_to_<unsigned char>());
      case IntegerType::USHORT: return format("{}", static_cast_to_<unsigned short>());
      case IntegerType::UINT: return format("{}", static_cast_to_<unsigned int>());
      case IntegerType::ULONG: return format("{}", static_cast_to_<unsigned long>());
      case IntegerType::ULONGLONG: return format("{}", static_cast_to_<unsigned long long>());
      }
      assert(false); // logic error
      return ""s;
   }

#define UNARY_OP(fname)                                                                    \
   constexpr Integer fname() const noexcept                                                \
   {                                                                                       \
      switch(type()) {                                                                     \
      case IntegerType::CHAR: return fname##_(static_cast_to_<char>());                    \
      case IntegerType::SHORT: return fname##_(static_cast_to_<short>());                  \
      case IntegerType::INT: return fname##_(static_cast_to_<int>());                      \
      case IntegerType::LONG: return fname##_(static_cast_to_<long>());                    \
      case IntegerType::LONGLONG: return fname##_(static_cast_to_<long long>());           \
      case IntegerType::UCHAR: return fname##_(static_cast_to_<unsigned char>());          \
      case IntegerType::USHORT: return fname##_(static_cast_to_<unsigned short>());        \
      case IntegerType::UINT: return fname##_(static_cast_to_<unsigned int>());            \
      case IntegerType::ULONG: return fname##_(static_cast_to_<unsigned long>());          \
      case IntegerType::ULONGLONG: return fname##_(static_cast_to_<unsigned long long>()); \
      }                                                                                    \
      assert(false); /* logic error */                                                     \
      return Integer{0};                                                                   \
   }

   UNARY_OP(unot)
   UNARY_OP(uplus)
   UNARY_OP(uminus)
   UNARY_OP(utilde)
#undef UNARY_OP

#define BINOP_INNER(OP, TYPE)                                                              \
   switch(rhs.type()) {                                                                    \
   case IntegerType::CHAR: return OP(lhs.sc_<TYPE>(), rhs.sc_<char>());                    \
   case IntegerType::SHORT: return OP(lhs.sc_<TYPE>(), rhs.sc_<short>());                  \
   case IntegerType::INT: return OP(lhs.sc_<TYPE>(), rhs.sc_<int>());                      \
   case IntegerType::LONG: return OP(lhs.sc_<TYPE>(), rhs.sc_<long>());                    \
   case IntegerType::LONGLONG: return OP(lhs.sc_<TYPE>(), rhs.sc_<long long>());           \
   case IntegerType::UCHAR: return OP(lhs.sc_<TYPE>(), rhs.sc_<unsigned char>());          \
   case IntegerType::USHORT: return OP(lhs.sc_<TYPE>(), rhs.sc_<unsigned short>());        \
   case IntegerType::UINT: return OP(lhs.sc_<TYPE>(), rhs.sc_<unsigned int>());            \
   case IntegerType::ULONG: return OP(lhs.sc_<TYPE>(), rhs.sc_<unsigned long>());          \
   case IntegerType::ULONGLONG: return OP(lhs.sc_<TYPE>(), rhs.sc_<unsigned long long>()); \
   }                                                                                       \
   assert(false); /* logic error */                                                        \
   return Integer{0};

#define BINARY_OPERATOR(fname)                                               \
   constexpr Integer fname(const Integer& rhs) const noexcept                \
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
   BINARY_OPERATOR(right_shift) BINARY_OPERATOR(less) BINARY_OPERATOR(less_eq)
       BINARY_OPERATOR(greater) BINARY_OPERATOR(greater_eq) BINARY_OPERATOR(equal)
           BINARY_OPERATOR(not_equal) BINARY_OPERATOR(bitwise_and) BINARY_OPERATOR(bitwise_xor)
               BINARY_OPERATOR(bitwise_or) BINARY_OPERATOR(logical_and) BINARY_OPERATOR(logical_or)

#undef BINARY_OPERATOR
#undef BINOP_INNER

                   private : template<typename T>
                             constexpr T static_cast_to_() const noexcept
   {
      switch(type()) {
      case IntegerType::CHAR: return static_cast<T>(data_.c);
      case IntegerType::SHORT: return static_cast<T>(data_.s);
      case IntegerType::INT: return static_cast<T>(data_.i);
      case IntegerType::LONG: return static_cast<T>(data_.l);
      case IntegerType::LONGLONG: return static_cast<T>(data_.ll);
      case IntegerType::UCHAR: return static_cast<T>(data_.uc);
      case IntegerType::USHORT: return static_cast<T>(data_.us);
      case IntegerType::UINT: return static_cast<T>(data_.ui);
      case IntegerType::ULONG: return static_cast<T>(data_.ul);
      case IntegerType::ULONGLONG: return static_cast<T>(data_.ull);
      }
      assert(false); // Logic error
      return T{0};
   }

   template<typename T> constexpr T sc_() const noexcept { return static_cast_to_<T>(); }

   template<typename T, typename F>
   static constexpr Integer curried_(T lhs, const Integer& rhs, F&& f) noexcept
   {
      switch(rhs.type()) {
      case IntegerType::CHAR: return Integer(f(lhs, rhs.static_cast_to_<char>));
      case IntegerType::SHORT: return Integer(f(lhs, rhs.static_cast_to_<short>));
      case IntegerType::INT: return Integer(f(lhs, rhs.static_cast_to_<int>));
      case IntegerType::LONG: return Integer(f(lhs, rhs.static_cast_to_<long>));
      case IntegerType::LONGLONG: return Integer(f(lhs, rhs.static_cast_to_<long long>));
      case IntegerType::UCHAR: return Integer(f(lhs, rhs.static_cast_to_<unsigned char>));
      case IntegerType::USHORT: return Integer(f(lhs, rhs.static_cast_to_<unsigned short>));
      case IntegerType::UINT: return Integer(f(lhs, rhs.static_cast_to_<unsigned int>));
      case IntegerType::ULONG: return Integer(f(lhs, rhs.static_cast_to_<unsigned long>));
      case IntegerType::ULONGLONG: return Integer(f(lhs, rhs.static_cast_to_<unsigned long long>));
      }
      assert(false); // Logic error
      return T{0};
   }

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

}; // namespace giraffe

} // namespace giraffe
