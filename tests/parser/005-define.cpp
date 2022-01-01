
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

constexpr static const char* test_text_005 = R"V0G0N(
#define
#define NDEBUG
#define FOO(a, b, c) some - tokens(a)
#define ZAPPY "Heh The Preprocessor!"
)V0G0N";

constexpr static const char* test_text_005_result =
    R"V0G0N(<empty-node>
#define NDEBUG
#define FOO(a, b, c) some - tokens(a)
#define ZAPPY "Heh The Preprocessor!"
)V0G0N";

constexpr auto test_003_token_seq = to_array<int>(
    {TNEWLINE, TERROR, TLINE_PART, TNEWLINE, TWARNING, TLINE_PART, TNEWLINE, TEOF});

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("005 define", "[005-define]")
{
   auto dump_parse = [&](Context& context) {
      auto& scanner = context.scanner();
      scanner.set_position(1); // Skip TSTART
      bool first = true;
      while(scanner.has_next()) {
         if(first)
            first = false;
         else
            cout << ", ";
         cout << token_id_to_str(scanner.consume().id());
      }
      cout << endl;

      scanner.set_position(1); // Skip TSTART
      while(scanner.has_next()) {
         const auto& token = scanner.consume();
         cout << format("{:15s} {:15s} {}\n",
                        str(token.location()),
                        token_id_to_str(token.id()),
                        encode_string(token.text()));
      }

      scanner.set_position(1); // Skip TSTART
      unique_ptr<AstNode> node(accept_stmt_list(context));
      node->stream(cout, 0);
   };

   auto context_ptr = Context::make(make_unique<Scanner>("test-005", test_text_005));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   dump_parse(context);
}

} // namespace giraffe::test
