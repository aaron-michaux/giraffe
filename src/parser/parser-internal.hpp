
#pragma once

#include "ast/ast.hpp"
#include "driver/context.hpp"
#include "scanner/scanner.hpp"

#include "skip-to-sequence.hpp"
#include "token-sets.hpp"

namespace giraffe
{
template<typename T> inline bool expect(Scanner& tokens, const T& ids) noexcept;
template<typename T> inline bool accept(Scanner& tokens, const T& ids) noexcept;

// Recovery functions
bool recover_to_next_rule(Scanner& tokens) noexcept;
bool skip_past_element(Scanner& tokens) noexcept;

// Accept functions -- could return `EmptyNode`
TranslationUnitNode* accept_translation_unit(Context& context) noexcept;
StmtListNode* accept_stmt_list(Context& context) noexcept;
ExpressionNode* accept_expression(Context& context) noexcept;

AstNode* accept_module(Context& context) noexcept;
AstNode* accept_if_then(Context& context) noexcept;
AstNode* accept_define(Context& context) noexcept;
AstNode* accept_undef(Context& context) noexcept;
AstNode* accept_include(Context& context) noexcept;
AstNode* accept_error(Context& context) noexcept;

std::pair<sso23::string, SourceRange> accept_cstr(Context& context) noexcept;
sso23::string accept_to_newline(Context& context) noexcept;

// Helper, for creating empty nodes as fillers for parser errors
inline EmptyNode* make_empty_node() noexcept { return new EmptyNode(); }

//
//
//
//
// Implementations Below
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// ------------- implementations

template<typename T> inline bool expect(Scanner& tokens, const T& ids) noexcept
{
   if constexpr(std::is_integral<std::remove_reference_t<T>>::value) {
      return tokens.current().id() == ids;
   } else {
      const int current_id = tokens.current().id();
      const auto ii        = std::find(cbegin(ids), cend(ids), current_id);
      return (ii != cend(ids));
   }
}

template<typename T> inline bool accept(Scanner& tokens, const T& ids) noexcept
{
   if(expect(tokens, ids)) {
      tokens.consume();
      return true;
   }
   return false;
}

} // namespace giraffe
