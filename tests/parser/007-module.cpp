
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

constexpr static const char* test_text_007 = R"V0G0N(
module;

// This is the global module fragment
// Only preprocessor commands allowed

// Regular module (appears ONCE per module)
export module speech;

// This is the module purview for 'speech'

// All imports must appear before and declarations
// Import must be at global scope
// No cyclic dependencies!
// Can only import module partitions belonging to the parent module
import speech;
import <iostream>;
import <=>;
import "some/stuff.hpp";

// Re-export interface partitions
export import :english;
export import :spanish;

// Submodule
export module speech:spanish;

// This is a "submodule" that is really a module
export module speech.english;

// A module "implementation" unit
module speech;

// The start of the "private" module fragment
module :private;

)V0G0N";

constexpr static const char* test_text_007_result = R"V0G0N(module;
export module speech;
import speech;
import <iostream>;
import <=>;
import "some/stuff.hpp";
export import :english;
export import :spanish;
export module speech:spanish;
export module speech.english;
module speech;
module :private;
)V0G0N";

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("007 module", "[module]")
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

   auto context_ptr = Context::make(make_unique<Scanner>("test-007", test_text_007));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   dump_parse(context);

   CATCH_SECTION("007")
   {
      {
         scanner.set_position(0);
         unique_ptr<AstNode> node(accept_stmt_list(context));

         std::stringstream ss;
         node->stream(ss, 0);
         CATCH_REQUIRE(ss.str() == test_text_007_result);
      }
      CATCH_REQUIRE(context.diagnostics().size() == 0);
      CATCH_REQUIRE(AstNode::get_node_count() == 0);
   }
}

} // namespace giraffe::test
