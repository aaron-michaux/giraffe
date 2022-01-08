
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

constexpr static const char* test_text_008 = R"V0G0N(
#define SOMETHING
#define
#define 1
#define X(a)
#define X(a, b) (a + b)
)V0G0N";

constexpr static const char* test_text_008_result = R"V0G0N(#define SOMETHING
<empty-node>
<empty-node>
#define X(a)
#define X(a, b) (a + b)
)V0G0N";

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("008 module", "[module]")
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

      {
         for(const auto& diag : context.diagnostics()) { diag.stream(cout, context); }
      }

      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   };

   auto context_ptr = Context::make(make_unique<Scanner>("test-008", test_text_008));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   // dump_parse(context);

   CATCH_SECTION("008")
   {
      {
         scanner.set_position(0);
         unique_ptr<AstNode> node(accept_stmt_list(context));

         std::stringstream ss;
         node->stream(ss, 0);
         CATCH_REQUIRE(ss.str() == test_text_008_result);
      }
      CATCH_REQUIRE(context.diagnostics().size() == 2);
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   }
}

} // namespace giraffe::test
