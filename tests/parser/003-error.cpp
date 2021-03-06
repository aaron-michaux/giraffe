
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

constexpr static const char* test_text_003 = R"V0G0N(
#error Here is some stuff that could be part of an error!
#warning "And \t\b\r\n\v\f\\\"'that was something!"
)V0G0N";

constexpr static const char* test_text_003_result =
    R"V0G0N(#error Here is some stuff that could be part of an error!
#warning "And \t\b\r\n\v\f\\\"'that was something!"
)V0G0N";

constexpr auto test_003_token_seq
    = to_array<int>({TNEWLINE, TERROR, TLINE_PART, TNEWLINE, TWARNING, TLINE_PART, TNEWLINE, TEOF});

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("003 errors", "[003-errors]")
{
   auto print_token = [](auto& os, const Token& token) {
      os << format("{:15s} {:15s} {}\n",
                   str(token.location()),
                   token_id_to_str(token.id()),
                   encode_string(token.text()));
   };

   auto context_ptr = Context::make(make_unique<Scanner>("test-003", test_text_003));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   if(false) {
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
      while(scanner.has_next()) { print_token(cout, scanner.consume()); }

      scanner.set_position(1); // Skip TSTART
      unique_ptr<StmtListNode> stmts(accept_stmt_list(context));
      stmts->stream(cout, 0);
   }

   // -----------------------------------
   CATCH_SECTION("test token sequence")
   {
      scanner.set_position(1); // Skip TSTART
      for(const auto token_id : test_003_token_seq) {
         CATCH_REQUIRE(scanner.consume().id() == token_id);
      }
      CATCH_REQUIRE(scanner.found_eof());
      CATCH_REQUIRE(scanner.reached_eof());
   }

   // -----------------------------------

   CATCH_SECTION("test parse-tree")
   {
      scanner.set_position(1); // Skip TSTART
      {
         unique_ptr<StmtListNode> stmts(accept_stmt_list(context));

         CATCH_REQUIRE(stmts->size() == 2);
         {
            auto child = stmts->children()[0];
            CATCH_REQUIRE(child->node_type() == NodeType::ERROR);
            auto node = cast_ast_node<ErrorNode>(child);
            CATCH_REQUIRE(node->is_error() == true);
            CATCH_REQUIRE(node->message() == " Here is some stuff that could be part of an error!");
         }

         {
            auto child = stmts->children()[1];
            CATCH_REQUIRE(child->node_type() == NodeType::ERROR);
            auto node = cast_ast_node<ErrorNode>(child);
            CATCH_REQUIRE(node->is_error() == false);
            CATCH_REQUIRE(node->message()
                          == " \"And \\t\\b\\r\\n\\v\\f\\\\\\\"'that was something!\"");
         }

         std::stringstream ss;
         stmts->stream(ss, 0);
         CATCH_REQUIRE(ss.str() == test_text_003_result);

         const Diagnostics& diagnostics = context.diagnostics();
         CATCH_REQUIRE(diagnostics.size() == 0);

         // Should not throw
         for(const auto& diagnostic : diagnostics) ss << diagnostic.to_string();

         // cout << ss.str()
      }
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   }
}

} // namespace giraffe::test
