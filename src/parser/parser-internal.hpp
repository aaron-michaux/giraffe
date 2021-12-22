
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

// Accept functions
TranslationUnitNode * accept_translation_unit(Context& context) noexcept;
ModuleNode * accept_module(Context& context) noexcept;
IfThenNode * accept_if_then(Context& context) noexcept;
CommandNode * accept_command(Context& context) noexcept;
ExpressionNode * accept_expression(Context& context) noexcept;

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
