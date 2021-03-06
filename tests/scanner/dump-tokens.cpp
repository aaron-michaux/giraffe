
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "scanner/scanner.hpp"

namespace giraffe::test
{

// ----------------------------------------------------- test-text-dump-tokens-1

constexpr static const char* test_text_dump_tokens_1 = R"V0G0N(

#pragma once

// C++ comment
/* A 
   C
comment */

  #include /* a comment */ <type_traits>

#if(defined __GLIBCXX__ && __GLIBCXX__ <= 20200422) || defined _LIBCPP_VERSION
#define USE_CONCEPT_FILLER__
#endif

#ifdef USE_CONCEPT_FILLER__
namespace std {
template<class T> concept integral        = is_integral_v<T>;
}
#endif

)V0G0N";

// ----------------------------------------------------------------- dump-tokens

CATCH_TEST_CASE("dump tokens", "[dump-tokens]")
{
   auto print_token = [](auto& os, const Token& token) {
      os << format("{:15s} {:15s} {}\n",
                   str(token.location()),
                   token_id_to_str(token.id()),
                   encode_string(token.text()));
   };

   CATCH_SECTION("some section")
   {
      ScannerOptions opts;
      opts.skip_newlines = true;
      Scanner scanner("test-text-dump-tokens-1", test_text_dump_tokens_1, opts);
      while(scanner.has_next()) {
         scanner.consume();
         // print_token(cout, scanner.current());
      }
      // print_token(cout, scanner.current());

      CATCH_REQUIRE(scanner.found_eof());
      CATCH_REQUIRE(scanner.reached_eof());
   }
}

} // namespace giraffe::test
