
#pragma once

#include "source-location.hpp"
#include "token-types.h"

namespace giraffe
{
struct Token;
using TokenTextFunctor = std::function<string_view(const Token&)>;

struct Token final
{
 private:
   String text_             = {};
   SourceLocation loc_      = {};
   uint8_t id_              = 0; // id of this token
   bool is_space_delimited_ = false;

 public:
   static Token make_start_token() noexcept { return Token(TSTART); }

   Token(uint8_t id = TNONE)
       : id_(id)
   {}

   Token(uint8_t id, SourceLocation loc, String text, bool space_delimited)
       : text_(text)
       , loc_(loc)
       , id_(id)
       , is_space_delimited_(space_delimited)
   {
      assert(id_ == id);
   }

   Token(const Token&)     = delete;
   Token(Token&&) noexcept = default;
   ~Token()                = default;
   Token& operator=(const Token&) = delete;
   Token& operator=(Token&&) noexcept = default;

   auto id() const noexcept { return id_; }
   auto location() const noexcept { return loc_; }
   auto length() const noexcept { return text_.size(); }
   auto offset() const noexcept { return loc_.offset; }
   auto line_no() const noexcept { return loc_.line_no; }
   auto text() const noexcept { return string_view(text_); }
   auto is_space_delimited() const noexcept
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

   bool is_none() const noexcept { return token_id_is_none(id()); }
   bool is_eof() const noexcept { return token_id_is_eof(id()); }
   bool is_keyword() const noexcept { return token_id_is_keyword(id()); }
   bool is_newline() const noexcept { return token_id_is_newline(id()); }
   bool is_badchar() const noexcept { return id() == TBADCHAR; }
   bool is_comment() const noexcept { return token_id_is_comment(id()); }
   bool is_whitespace() const noexcept { return token_id_is_whitespace(id()); }
};

} // namespace giraffe
