
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "scanner/scanner.hpp"
#include "parser/parser-internal.hpp"
#include "driver/context.hpp"

namespace giraffe::test {

// ----------------------------------------------------- test-text-dump-tokens-1
/**
 * Test #include directive, with comments interspersed
 */
constexpr static const char * test_text_001 = R"V0G0N(

/* include a file, pathy path */ #include  "file0"
#include /* include a file, pathy path */ "file1"
#include <file2> /* incude a file, system path */
#include <=> // Include the spaceship

)V0G0N";

constexpr auto test_001_token_seq = to_array<int>({
      TNEWLINE, TNEWLINE, TINCLUDE, TSTR_DELIM, TSTR_PART, TSTR_DELIM, TNEWLINE,
      TINCLUDE, TSTR_DELIM, TSTR_PART, TSTR_DELIM, TNEWLINE, TINCLUDE, TSTRING,
      TNEWLINE, TINCLUDE, TSPACESHIP, TNEWLINE, TNEWLINE, TEOF
   });

// ----------------------------------------------------------------- dump-tokens

CATCH_TEST_CASE("001 include", "[001-include]")
{
   auto print_token = [] (auto& os, const Token& token) {
      os << format("{:15s} {:15s} {}\n",
                   str(token.location()),
                   token_id_to_str(token.id()),
      encode_string(token.text()));
   };

   auto context_ptr = Context::make(make_unique<Scanner>("test-001", test_text_001));
   auto& context = *context_ptr;
   auto& scanner = context.scanner();
   
   // -----------------------------------
   CATCH_SECTION("test token sequence") {
      scanner.set_position(1); // Skip TSTART
      for(const auto token_id: test_001_token_seq) {
         CATCH_REQUIRE(scanner.consume().id() == token_id);
      }
      CATCH_REQUIRE(scanner.found_eof());
      CATCH_REQUIRE(scanner.reached_eof());
   }

   // -----------------------------------
   CATCH_SECTION("test parse-tree") {
      scanner.set_position(1); // Skip TSTART
      unique_ptr<StmtListNode> stmts(accept_stmt_list(context));
      stmts->stream(cout, 0);
   }

}

}

