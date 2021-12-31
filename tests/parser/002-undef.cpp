
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
constexpr static const char * test_text_002 = R"V0G0N(
#undef BEATLEJUICE
#undef 0
)V0G0N";

constexpr static const char * test_text_002_result =
   R"V0G0N(#undef BEATLEJUICE
<empty-node>
)V0G0N";

constexpr auto test_002_token_seq = to_array<int>({
      TNEWLINE, TUNDEF, TIDENTIFIER, TNEWLINE, TUNDEF, TINTEGER, TNEWLINE, TEOF
   });

// ----------------------------------------------------------------- dump-tokens

CATCH_TEST_CASE("002 undef", "[002-undef]")
{
   auto print_token = [] (auto& os, const Token& token) {
      os << format("{:15s} {:15s} {}\n",
                   str(token.location()),
                   token_id_to_str(token.id()),
      encode_string(token.text()));
   };

   auto context_ptr = Context::make(make_unique<Scanner>("test-002", test_text_002));
   auto& context = *context_ptr;
   auto& scanner = context.scanner();

   while(scanner.has_next()) {
      //cout << token_id_to_str(scanner.consume().id()) << endl;
      print_token(cout, scanner.consume());
   }
   
   // -----------------------------------
   CATCH_SECTION("test token sequence") {
      scanner.set_position(1); // Skip TSTART      
      for(const auto token_id: test_002_token_seq) {
         CATCH_REQUIRE(scanner.consume().id() == token_id);
      }
      CATCH_REQUIRE(scanner.found_eof());
      CATCH_REQUIRE(scanner.reached_eof());
   }

   // -----------------------------------
   CATCH_SECTION("test parse-tree") {
      scanner.set_position(1); // Skip TSTART
      {
         unique_ptr<StmtListNode> stmts(accept_stmt_list(context));

         CATCH_REQUIRE(stmts->size() == 2);
         {
            auto child = stmts->children()[0];
            CATCH_REQUIRE(child->node_type() == NodeType::UNDEF);
            auto node = cast_ast_node<UndefNode>(child);
            CATCH_REQUIRE(node->identifier() == "BEATLEJUICE");
         }

         {
            auto child = stmts->children()[1];
            CATCH_REQUIRE(child->node_type() == NodeType::EMPTY); // and error
         }
         
         std::stringstream ss;
         stmts->stream(ss, 0);
         cout << ss.str() << endl;
         CATCH_REQUIRE(ss.str() == test_text_002_result);
      }
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   }

}

}

