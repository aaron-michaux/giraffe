
#include "stdinc.hpp"

#include <catch2/catch.hpp>

#include "driver/context.hpp"
#include "parser/parser-internal.hpp"
#include "scanner/scanner.hpp"

namespace giraffe::test
{

// -----------------------------------------------------------------------------

constexpr static const char* test_text_006 = R"V0G0N(
#pragma once

#if !defined NDEBUG
#include <cstdio>
#endif

#ifndef INCLUDE_GUARD
#define INCLUDE_GUARD

#ifdef __cplusplus
#if VERSION >= 20211201ull
#include "assert.h"
#else
#error "Done Bad"
#endif
#else
#include <cassert>
#endif

#endif

)V0G0N";

constexpr static const char* test_text_006_result =
    R"V0G0N(
)V0G0N";

// -------------------------------------------------------------------- testcase

CATCH_TEST_CASE("006 if then", "[006-if-then]")
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

   auto context_ptr = Context::make(make_unique<Scanner>("test-006", test_text_006));
   auto& context    = *context_ptr;
   auto& scanner    = context.scanner();

   dump_parse(context);
}

} // namespace giraffe::test
