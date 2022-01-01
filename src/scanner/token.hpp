
#pragma once

#include <sso-string/string.hpp>

#include "source-location.hpp"
#include "token-types.h"

namespace giraffe
{
struct Token;
using TokenTextFunctor = std::function<string_view(const Token&)>;
using SsoString = sso23::basic_string<char>;

struct Token final
{
 private:
   // For our lexer, most tokens are 1 character, and are rarely longer than
   // 23 characters. Using short-string-optimization (gcc+clang) is important.
   SsoString text_          = {};    
   SourceLocation loc_      = {};
   uint8_t id_              = TNONE; // id of this token
   bool is_space_delimited_ = false;

 public:
   static Token make_start_token() noexcept { return Token(TSTART); }

   Token(uint8_t id = TNONE)
       : id_(id)
   {}

   Token(uint8_t id, SourceLocation loc, string_view text, bool space_delimited)
      : text_{text.data(), text.size()}
      , loc_{loc}
      , id_{id}
      , is_space_delimited_{space_delimited}
   {
      assert(id_ == id);
   }

   constexpr Token(const Token&)     = delete;
   Token(Token&&) noexcept = default;
   ~Token()                = default;
   constexpr Token& operator=(const Token&) = delete;
   Token& operator=(Token&&) noexcept = default;

   constexpr auto id() const noexcept { return id_; }
   constexpr auto location() const noexcept { return loc_; }
   auto length() const noexcept { return text_.size(); }
   constexpr auto offset() const noexcept { return loc_.offset; }
   constexpr auto line_no() const noexcept { return loc_.line_no; }
   auto text() const noexcept { return string_view{text_.data(), text_.size()}; }
   constexpr auto is_space_delimited() const noexcept
   {
      return is_space_delimited_;
   }

   /// The source location just past the end of the token
   auto end_location() const noexcept
   {
      SourceLocation loc = loc_;
      loc.offset += length();
      assert(loc.offset == loc_.offset + length());
      return loc;
   }

   auto source_range() const noexcept
   {
      return SourceRange{location(), end_location()};
   }

   // -- Methods -- //

   constexpr bool is_none() const noexcept { return token_id_is_none(id()); }
   constexpr bool is_eof() const noexcept { return token_id_is_eof(id()); }
   constexpr bool is_keyword() const noexcept { return token_id_is_keyword(id()); }
   constexpr bool is_newline() const noexcept { return token_id_is_newline(id()); }
   constexpr bool is_badchar() const noexcept { return id() == TBADCHAR; }
   constexpr bool is_comment() const noexcept { return token_id_is_comment(id()); }
   constexpr bool is_whitespace() const noexcept { return token_id_is_whitespace(id()); }
};

} // namespace giraffe
