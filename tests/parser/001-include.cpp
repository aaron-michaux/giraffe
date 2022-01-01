
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

/**
 * Test #include directive, with comments interspersed
 */
constexpr static const char* test_text_001 = R"V0G0N(

/* include a file, pathy path */ #include  "file0"
#include " \"!"
#include /* include a file, pathy path */ "file1"
#include <=> // Include the spaceship
#include <file2> /* incude a file, system path */

)V0G0N";

constexpr static const char* test_text_001_result =
    R"V0G0N(#include "file0"
#include " \"!"
#include "file1"
#include <=>
#include <file2>
)V0G0N";

constexpr auto test_001_token_seq = to_array<int>(
    {TNEWLINE, TNEWLINE,   TINCLUDE,  TSTR_DELIM, TSTR_PART, TSTR_DELIM, TNEWLINE,
     TINCLUDE, TSTR_DELIM, TSTR_PART, TSTR_PART,  TSTR_PART, TSTR_DELIM, TNEWLINE,
     TINCLUDE, TSTR_DELIM, TSTR_PART, TSTR_DELIM, TNEWLINE,  TINCLUDE,   TSPACESHIP,
     TNEWLINE, TINCLUDE,   TSTRING,   TNEWLINE,   TNEWLINE,  TEOF});

constexpr auto test_001_results = to_array<std::pair<string_view, bool>>(
    {{"file0", true}, {" \"!", true}, {"file1", true}, {"=", false}, {"file2", false}});

// ----------------------------------------------------------------- dump-tokens

CATCH_TEST_CASE("001 include", "[001-include]")
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

   auto context_ptr = Context::make(make_unique<Scanner>("test-001", test_text_001));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   if(false) { dump_parse(context); }

   // -----------------------------------
   CATCH_SECTION("test token sequence")
   {
      scanner.set_position(1); // Skip TSTART
      for(const auto token_id : test_001_token_seq) {
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

         CATCH_REQUIRE(stmts->size() == 5);
         for(auto i = 0; i < 5; ++i) {
            auto child = stmts->children()[i];
            CATCH_REQUIRE(child->node_type() == NodeType::INCLUDE);
            auto inc_node                           = cast_ast_node<IncludeNode>(child);
            const auto [filename, is_local_include] = test_001_results[i];
            if(false) {
               TRACE(format("#{}: {}:{}, {}:{}",
                            i,
                            inc_node->filename(),
                            inc_node->is_local_include(),
                            filename,
                            is_local_include));
            }
            CATCH_REQUIRE((inc_node->filename() == filename));
            CATCH_REQUIRE((inc_node->is_local_include() == is_local_include));
         }

         std::stringstream ss;
         stmts->stream(ss, 0);
         CATCH_REQUIRE(ss.str() == test_text_001_result);
      }
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   }
}

} // namespace giraffe::test
