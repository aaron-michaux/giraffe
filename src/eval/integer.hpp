
#pragma once

namespace giraffe
{
/** The different integer types */
enum class IntegerType : uint8_t {
   CHAR      = 0, // Always signed
   SHORT     = 1,
   INT       = 2,
   LONG      = 3,
   LONGLONG  = 4,
   UCHAR     = 5,
   USHORT    = 6,
   UINT      = 7,
   ULONG     = 8,
   ULONGLONG = 9
};

} // namespace giraffe

namespace giraffe::detail
{
// -- The size of a given IntegerType (calculated at runtime)
constexpr std::size_t int_size_(IntegerType type) noexcept;
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
   bool is_valid_    = true; //!< If `false` then the integer is invalid

 public:
   constexpr Integer() noexcept
       : Integer(0)
   {}

#define EXPLICIT_CONST(T, member, type)         \
   constexpr explicit Integer(T value) noexcept \
       : data_{.member = {value}}               \
       , type_{IntegerType::type}               \
       , is_valid_{true}                        \
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

   static Integer make_invalid() noexcept;

   constexpr auto is_valid() const noexcept { return is_valid_; }
   constexpr auto is_invalid() const noexcept { return !is_valid_; }
   constexpr auto type() const noexcept { return type_; }
   constexpr auto data() const noexcept { return data_; }

   bool is_signed() const noexcept;
   int64_t signed_value() const noexcept;
   uint64_t unsigned_value() const noexcept;

   bool is_true() const noexcept
   {
      return is_signed() ? (signed_value() != 0) : (unsigned_value() != 0);
   }
   bool is_false() const noexcept { return !is_true(); }

   bool operator==(const Integer& rhs) const noexcept;
   bool operator!=(const Integer& rhs) const noexcept { return !(*this == rhs); }

   std::string to_string() const noexcept;

   //@{ Unary operators
   Integer unot() const noexcept;
   Integer uplus() const noexcept;
   Integer uminus() const noexcept;
   Integer utilde() const noexcept;
   //@}

   //@{ Binary operators
   Integer plus(const Integer& rhs) const noexcept;
   Integer minus(const Integer& rhs) const noexcept;
   Integer multiply(const Integer& rhs) const noexcept;
   Integer divide(const Integer& rhs) const noexcept;
   Integer remainder(const Integer& rhs) const noexcept;
   Integer left_shift(const Integer& rhs) const noexcept;
   Integer right_shift(const Integer& rhs) const noexcept;
   Integer less(const Integer& rhs) const noexcept;
   Integer less_eq(const Integer& rhs) const noexcept;
   Integer greater(const Integer& rhs) const noexcept;
   Integer greater_eq(const Integer& rhs) const noexcept;
   Integer equal(const Integer& rhs) const noexcept;
   Integer not_equal(const Integer& rhs) const noexcept;
   Integer bitwise_and(const Integer& rhs) const noexcept;
   Integer bitwise_xor(const Integer& rhs) const noexcept;
   Integer bitwise_or(const Integer& rhs) const noexcept;
   Integer logical_and(const Integer& rhs) const noexcept;
   Integer logical_or(const Integer& rhs) const noexcept;
   //@}
};

} // namespace giraffe
