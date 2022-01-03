

#include "stdinc.hpp"

#include <random>

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"
#include "sema/integer.hpp"

namespace giraffe::test
{
namespace detail020
{
   template<typename T> void test_type(IntegerType type)
   {
      const T x = 0;
      const Integer i{x};
      CATCH_REQUIRE(i.type() == type);
      CATCH_REQUIRE(detail::is_signed_(type) == std::is_signed<T>::value);
      CATCH_REQUIRE(i.signed_value() == x);
      CATCH_REQUIRE(i.unsigned_value() == x);
   }

   template<typename U, typename V> void cross_test(auto& gen)
   {
      std::uniform_int_distribution<U> udist;
      std::uniform_int_distribution<V> vdist;

      for(auto i = 1; i < 10; ++i) {
         const auto u = static_cast<U>(udist(gen));
         const auto v = static_cast<V>(vdist(gen));
         const auto a = Integer{u};
         const auto b = Integer{v};

         CATCH_REQUIRE(a.to_string() == format("{}", u));
         CATCH_REQUIRE(b.to_string() == format("{}", v));

#define CASE(op, func)                          \
   CATCH_REQUIRE(Integer{u op v} == a.func(b)); \
   CATCH_REQUIRE(a.func(b).to_string() == format("{}", u op v));

         CASE(+, plus)
         CASE(-, minus)
         CASE(*, multiply)
         CASE(/, divide)
         CASE(%, remainder)
#undef CASE
      }
   }
} // namespace detail020

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("020 Integer", "[020-integer]")
{
   std::mt19937_64 gen64;
   gen64.seed(1);

   CATCH_SECTION("020-Construction")
   {
      detail020::test_type<char>(IntegerType::CHAR);
      detail020::test_type<short>(IntegerType::SHORT);
      detail020::test_type<int>(IntegerType::INT);
      detail020::test_type<long>(IntegerType::LONG);
      detail020::test_type<long long>(IntegerType::LONGLONG);
      detail020::test_type<unsigned char>(IntegerType::UCHAR);
      detail020::test_type<unsigned short>(IntegerType::USHORT);
      detail020::test_type<unsigned int>(IntegerType::UINT);
      detail020::test_type<unsigned long>(IntegerType::ULONG);
      detail020::test_type<unsigned long long>(IntegerType::ULONGLONG);
   }

   CATCH_SECTION("020-Construction")
   {
      // char
      detail020::cross_test<char, char>(gen64);
      detail020::cross_test<char, short>(gen64);
      detail020::cross_test<char, int>(gen64);
      detail020::cross_test<char, long>(gen64);
      detail020::cross_test<char, long long>(gen64);
      detail020::cross_test<char, unsigned char>(gen64);
      detail020::cross_test<char, unsigned short>(gen64);
      detail020::cross_test<char, unsigned int>(gen64);
      detail020::cross_test<char, unsigned long>(gen64);
      detail020::cross_test<char, unsigned long long>(gen64);

      // short
      detail020::cross_test<short, char>(gen64);
      detail020::cross_test<short, short>(gen64);
      detail020::cross_test<short, int>(gen64);
      detail020::cross_test<short, long>(gen64);
      detail020::cross_test<short, long long>(gen64);
      detail020::cross_test<short, unsigned char>(gen64);
      detail020::cross_test<short, unsigned short>(gen64);
      detail020::cross_test<short, unsigned int>(gen64);
      detail020::cross_test<short, unsigned long>(gen64);
      detail020::cross_test<short, unsigned long long>(gen64);

      // int
      detail020::cross_test<int, char>(gen64);
      detail020::cross_test<int, short>(gen64);
      detail020::cross_test<int, int>(gen64);
      detail020::cross_test<int, long>(gen64);
      detail020::cross_test<int, long long>(gen64);
      detail020::cross_test<int, unsigned char>(gen64);
      detail020::cross_test<int, unsigned short>(gen64);
      detail020::cross_test<int, unsigned int>(gen64);
      detail020::cross_test<int, unsigned long>(gen64);
      detail020::cross_test<int, unsigned long long>(gen64);

      // long
      detail020::cross_test<long, char>(gen64);
      detail020::cross_test<long, short>(gen64);
      detail020::cross_test<long, int>(gen64);
      detail020::cross_test<long, long>(gen64);
      detail020::cross_test<long, long long>(gen64);
      detail020::cross_test<long, unsigned char>(gen64);
      detail020::cross_test<long, unsigned short>(gen64);
      detail020::cross_test<long, unsigned int>(gen64);
      detail020::cross_test<long, unsigned long>(gen64);
      detail020::cross_test<long, unsigned long long>(gen64);

      // long long
      detail020::cross_test<long long, char>(gen64);
      detail020::cross_test<long long, short>(gen64);
      detail020::cross_test<long long, int>(gen64);
      detail020::cross_test<long long, long>(gen64);
      detail020::cross_test<long long, long long>(gen64);
      detail020::cross_test<long long, unsigned char>(gen64);
      detail020::cross_test<long long, unsigned short>(gen64);
      detail020::cross_test<long long, unsigned int>(gen64);
      detail020::cross_test<long long, unsigned long>(gen64);
      detail020::cross_test<long long, unsigned long long>(gen64);

      // unsigned char
      detail020::cross_test<unsigned char, char>(gen64);
      detail020::cross_test<unsigned char, short>(gen64);
      detail020::cross_test<unsigned char, int>(gen64);
      detail020::cross_test<unsigned char, long>(gen64);
      detail020::cross_test<unsigned char, long long>(gen64);
      detail020::cross_test<unsigned char, unsigned char>(gen64);
      detail020::cross_test<unsigned char, unsigned short>(gen64);
      detail020::cross_test<unsigned char, unsigned int>(gen64);
      detail020::cross_test<unsigned char, unsigned long>(gen64);
      detail020::cross_test<unsigned char, unsigned long long>(gen64);

      // unsigned short
      detail020::cross_test<unsigned short, char>(gen64);
      detail020::cross_test<unsigned short, short>(gen64);
      detail020::cross_test<unsigned short, int>(gen64);
      detail020::cross_test<unsigned short, long>(gen64);
      detail020::cross_test<unsigned short, long long>(gen64);
      detail020::cross_test<unsigned short, unsigned char>(gen64);
      detail020::cross_test<unsigned short, unsigned short>(gen64);
      detail020::cross_test<unsigned short, unsigned int>(gen64);
      detail020::cross_test<unsigned short, unsigned long>(gen64);
      detail020::cross_test<unsigned short, unsigned long long>(gen64);

      // unsigned int
      detail020::cross_test<unsigned int, char>(gen64);
      detail020::cross_test<unsigned int, short>(gen64);
      detail020::cross_test<unsigned int, int>(gen64);
      detail020::cross_test<unsigned int, long>(gen64);
      detail020::cross_test<unsigned int, long long>(gen64);
      detail020::cross_test<unsigned int, unsigned char>(gen64);
      detail020::cross_test<unsigned int, unsigned short>(gen64);
      detail020::cross_test<unsigned int, unsigned int>(gen64);
      detail020::cross_test<unsigned int, unsigned long>(gen64);
      detail020::cross_test<unsigned int, unsigned long long>(gen64);

      // unsigned long
      detail020::cross_test<unsigned long, char>(gen64);
      detail020::cross_test<unsigned long, short>(gen64);
      detail020::cross_test<unsigned long, int>(gen64);
      detail020::cross_test<unsigned long, long>(gen64);
      detail020::cross_test<unsigned long, long long>(gen64);
      detail020::cross_test<unsigned long, unsigned char>(gen64);
      detail020::cross_test<unsigned long, unsigned short>(gen64);
      detail020::cross_test<unsigned long, unsigned int>(gen64);
      detail020::cross_test<unsigned long, unsigned long>(gen64);
      detail020::cross_test<unsigned long, unsigned long long>(gen64);

      // unsigned long long
      detail020::cross_test<unsigned long long, char>(gen64);
      detail020::cross_test<unsigned long long, short>(gen64);
      detail020::cross_test<unsigned long long, int>(gen64);
      detail020::cross_test<unsigned long long, long>(gen64);
      detail020::cross_test<unsigned long long, long long>(gen64);
      detail020::cross_test<unsigned long long, unsigned char>(gen64);
      detail020::cross_test<unsigned long long, unsigned short>(gen64);
      detail020::cross_test<unsigned long long, unsigned int>(gen64);
      detail020::cross_test<unsigned long long, unsigned long>(gen64);
      detail020::cross_test<unsigned long long, unsigned long long>(gen64);
   }
}

} // namespace giraffe::test
