
#include "stdinc.hpp"

#include <tuple>

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

const char* test_005_expr_01 = "1";
const char* test_005_expr_02 = "()";
const char* test_005_expr_03 = "((a))";
const char* test_005_expr_04 = "a";
const char* test_005_expr_05 = "(a)";
const char* test_005_expr_06 = "a + b";
const char* test_005_expr_07 = "(a + b)";
const char* test_005_expr_08 = "a * b + c";
const char* test_005_expr_09 = "(a * b + c)";
const char* test_005_expr_10 = "a * (b + c)";

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("005 undef", "[005-undef]")
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

      scanner.set_position(2); // Skip TSTART, TIF
      unique_ptr<ExpressionNode> expr(accept_expression(context));
      expr->stream(cout, 0);
   };

   auto run_test = [&](string_view name, string_view input_text, auto f) {
      const auto parse_text = format("#if {}", input_text);
      auto context_ptr      = Context::make(make_unique<Scanner>(name, parse_text));
      Context& context      = *context_ptr;
      Scanner& scanner      = context.scanner();
      // dump_parse(context);

      {
         scanner.set_position(1);
         CATCH_REQUIRE(scanner.consume().id() == TIF); // Skip the #if token
         CATCH_REQUIRE(expect(scanner, first_set_expressions));
         unique_ptr<ExpressionNode> expr(accept_expression(context));
         CATCH_REQUIRE(expr != nullptr);

         std::stringstream ss;
         expr->stream(ss, 0);
         f(context, expr.get(), ss.str(), context.diagnostics());
      }
      CATCH_REQUIRE(AstNode::get_node_count() == 0);

      {
         const Diagnostics& diagnostics = context.diagnostics();

         // Should not throw
         std::stringstream ss;
         for(const auto& diagnostic : diagnostics) diagnostic.stream(ss, context);
         // cout << ss.str()
      }
   };

   CATCH_SECTION("expr_01") // -------------------------------------------------
   {
      run_test("expr_01",
               test_005_expr_01,
               [](Context& context,
                  ExpressionNode* expr,
                  string_view str,
                  const Diagnostics& diagnostics) {
                  for(auto& diag : diagnostics) diag.stream(cout, context);
                  CATCH_REQUIRE(str == "1");
                  CATCH_REQUIRE(expr->size() == 0);
                  CATCH_REQUIRE(expr->expr_type() == ExprType::INTEGER);
                  CATCH_REQUIRE(expr->text() == "1");
                  CATCH_REQUIRE(diagnostics.size() == 0);
               });
   }

   CATCH_SECTION("expr_02") // -------------------------------------------------
   {
      run_test("expr_02",
               test_005_expr_02,
               [](Context& context,
                  ExpressionNode* expr,
                  string_view str,
                  const Diagnostics& diagnostics) {
                  // for(auto& diag : diagnostics) diag.stream(cout, context);
                  CATCH_REQUIRE(str == "");
                  CATCH_REQUIRE(expr->expr_type() == ExprType::EMPTY);
                  CATCH_REQUIRE(expr->size() == 0);
                  CATCH_REQUIRE(diagnostics.size() == 1);
                  CATCH_REQUIRE(diagnostics.totals().errors == 1);
                  CATCH_REQUIRE(context.has_error());
               });
   }

   CATCH_SECTION("expr_03") // -------------------------------------------------
   {
      run_test("expr_03",
               test_005_expr_03,
               [](Context& context,
                  ExpressionNode* expr,
                  string_view str,
                  const Diagnostics& diagnostics) {
                  CATCH_REQUIRE(str == "((a))");
                  CATCH_REQUIRE(expr->size() == 1);
                  CATCH_REQUIRE(expr->expr_type() == ExprType::SUBEXPR);
                  CATCH_REQUIRE(expr->text() == "");
                  auto child0 = expr->child(0);
                  CATCH_REQUIRE(child0->size() == 1);
                  CATCH_REQUIRE(child0->expr_type() == ExprType::SUBEXPR);
                  CATCH_REQUIRE(child0->text() == "");
                  auto child1 = child0->child(0);
                  CATCH_REQUIRE(child1->size() == 0);
                  CATCH_REQUIRE(child1->expr_type() == ExprType::IDENTIFIER);
                  CATCH_REQUIRE(child1->text() == "a");
                  CATCH_REQUIRE(diagnostics.size() == 0);
                  CATCH_REQUIRE(!context.has_error());
               });
   }

   CATCH_SECTION("expr_04") // -------------------------------------------------
   {
      run_test("expr_04",
               test_005_expr_04,
               [](Context& context,
                  ExpressionNode* expr,
                  string_view str,
                  const Diagnostics& diagnostics) {
                  CATCH_REQUIRE(str == "a");
                  CATCH_REQUIRE(expr->size() == 0);
                  CATCH_REQUIRE(expr->expr_type() == ExprType::IDENTIFIER);
                  CATCH_REQUIRE(expr->text() == "a");
                  CATCH_REQUIRE(diagnostics.size() == 0);
                  CATCH_REQUIRE(!context.has_error());
               });
   }

   CATCH_SECTION("expr_05") // -------------------------------------------------
   {
      run_test("expr_05",
               test_005_expr_05,
               [](Context& context,
                  ExpressionNode* expr,
                  string_view str,
                  const Diagnostics& diagnostics) {
                  CATCH_REQUIRE(str == "(a)");
                  CATCH_REQUIRE(expr->size() == 1);
                  CATCH_REQUIRE(expr->expr_type() == ExprType::SUBEXPR);
                  CATCH_REQUIRE(expr->op() == TNONE);
                  CATCH_REQUIRE(expr->text() == "");
                  auto child1 = expr->child(0);
                  CATCH_REQUIRE(child1->size() == 0);
                  CATCH_REQUIRE(child1->expr_type() == ExprType::IDENTIFIER);
                  CATCH_REQUIRE(child1->text() == "a");
                  CATCH_REQUIRE(diagnostics.size() == 0);
                  CATCH_REQUIRE(!context.has_error());
                  CATCH_REQUIRE(diagnostics.size() == 0);
                  CATCH_REQUIRE(!context.has_error());
               });
   }

   CATCH_SECTION("expr_06") // -------------------------------------------------
   {
      run_test("expr_06",
               test_005_expr_06,
               [](Context& context,
                  ExpressionNode* expr,
                  string_view str,
                  const Diagnostics& diagnostics) {
                  CATCH_REQUIRE(str == "a + b");
                  CATCH_REQUIRE(expr->size() == 2);
                  CATCH_REQUIRE(expr->expr_type() == ExprType::BINARY);
                  CATCH_REQUIRE(expr->op() == TPLUS);
                  CATCH_REQUIRE(expr->text() == "");
                  auto lhs = expr->lhs();
                  CATCH_REQUIRE(lhs->size() == 0);
                  CATCH_REQUIRE(lhs->expr_type() == ExprType::IDENTIFIER);
                  CATCH_REQUIRE(lhs->op() == TNONE);
                  CATCH_REQUIRE(lhs->text() == "a");
                  auto rhs = expr->rhs();
                  CATCH_REQUIRE(rhs->size() == 0);
                  CATCH_REQUIRE(rhs->expr_type() == ExprType::IDENTIFIER);
                  CATCH_REQUIRE(rhs->op() == TNONE);
                  CATCH_REQUIRE(rhs->text() == "b");
                  CATCH_REQUIRE(diagnostics.size() == 0);
                  CATCH_REQUIRE(!context.has_error());
               });
   }
}

} // namespace giraffe::test
