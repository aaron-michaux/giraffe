
#include "stdinc.hpp"

#include <catch2/catch.hpp>

using namespace giraffe;

CATCH_TEST_CASE("sample test case", "[sample]")
{
   CATCH_REQUIRE(0 < 1);

   CATCH_SECTION("some section") {
      CATCH_REQUIRE(string_view("").size() == 0);
   }
}

