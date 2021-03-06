
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

constexpr static const char* test_text_005z = R"V0G0N(
#define
#define NDEBUG
#define FOO(a, b, c) some - tokens(a)
#define ZAPPY "Heh The Preprocessor!"

#  define _GNUC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
)V0G0N";

constexpr static const char* test_text_005 = R"V0G0N(
#  define _GNUC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
)V0G0N";

constexpr static const char* test_text_005_result =
    R"V0G0N(<empty-node>
#define NDEBUG
#define FOO(a, b, c) some - tokens(a)
#define ZAPPY "Heh The Preprocessor!"
)V0G0N";

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

      {
         scanner.set_position(1); // Skip TSTART
         unique_ptr<AstNode> node(accept_stmt_list(context));
         node->stream(cout, 0);
      }
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   };

   auto context_ptr = Context::make(make_unique<Scanner>("test-005", test_text_005));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   CATCH_SECTION("005-define")
   {
      if(false) {
         scanner.set_position(0);
         unique_ptr<AstNode> node(accept_stmt_list(context));

         std::stringstream ss;
         node->stream(ss, 0);
         CATCH_REQUIRE(test_text_005_result == ss.str());
      }
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   }
}

} // namespace giraffe::test
