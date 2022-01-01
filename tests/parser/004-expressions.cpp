
#include "stdinc.hpp"

#include <tuple>

#include <catch2/catch.hpp>

#include "scanner/scanner.hpp"
#include "parser/parser-internal.hpp"
#include "driver/context.hpp"

namespace giraffe::test {

// -----------------------------------------------------------------------------

const char * test_005_expr_01 = "";
const char * test_005_expr_02 = "()";
const char * test_005_expr_03 = "((a))";
const char * test_005_expr_04 = "a";
const char * test_005_expr_05 = "(a)";
const char * test_005_expr_06 = "a + b";
const char * test_005_expr_07 = "(a + b)";
const char * test_005_expr_08 = "a * b + c";
const char * test_005_expr_09 = "(a * b + c)";
const char * test_005_expr_10 = "a * (b + c)";

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("005 undef", "[005-undef]")
{
   auto dump_parse = [&] (Context& context) {
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

      scanner.set_position(2); // Skip TSTART, TIF 
      unique_ptr<ExpressionNode> expr(accept_expression(context));
      expr->stream(cout, 0);
   };

   auto run_test = [&] (string_view name,
                        string_view input_text)
   {
      const auto parse_text = format("#if {}", input_text);
      auto context_ptr = Context::make(make_unique<Scanner>(name, parse_text));
      Context& context = *context_ptr;
      Scanner& scanner = context.scanner();
      dump_parse(context);
      
      {
         scanner.set_position(1);
         CATCH_REQUIRE(scanner.consume().id() == TIF); // Skip the #if token
         CATCH_REQUIRE(expect(scanner, first_set_expressions));
         unique_ptr<ExpressionNode> expr(accept_expression(context));

         std::stringstream ss;
         expr->stream(ss, 0);         
         // CATCH_REQUIRE(ss.str() == test_text_003_result);
      }  
      CATCH_REQUIRE(AstNode::get_node_count() == 0);

      {
         const Diagnostics& diagnostics = context.diagnostics();
         CATCH_REQUIRE(diagnostics.size() == 0);

         // Should not throw
         std::stringstream ss;
         for(const auto& diagnostic: diagnostics)
            diagnostic.stream(ss, context);
         // cout << ss.str()
      }
   };

   CATCH_SECTION("expr_04") { run_test("expr_04", test_005_expr_04); }
}

}



