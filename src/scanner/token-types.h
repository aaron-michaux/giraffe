
#pragma once

#include <stdint.h>

#define TNONE 0

/* occurs at the start of every (new) file */
#define TSTART 1

/* occurs ONCE after all inputs have been processed */
#define TEOF 2

#define TWHITESPACE 3

#define TNEWLINE 4
#define TNEWLINE_ESC 5

#define TIN_COMMENT_NEWLINE 6
#define TCOMMENT 7
#define TCOMMENT_DELIM 8
#define TCOMMENT_END_DELIM 9
#define TCOMMENT_PART 10

#define TLINE_PART 11

#define THASH 12

/* ----------------------------- Keywords */
#define TIF 13
#define TIFDEF 14
#define TIFNDEF 15
#define TELIF 16
#define TELSE 17
#define TENDIF 18

#define TINCLUDE 20
#define TDEFINE 21
#define TUNDEF 22

/* ----------------------------- Stuff */

#define TIDENTIFIER 30
#define TSTRING 31
#define TSTR_PART 32
#define TSTR_DELIM 33
#define TCSTRING 34
#define TSTRINGIFY 35
#define TDIGIT 36

#define TLT 40
#define TGT 41
#define TLE 42
#define TGE 43
#define TEQEQ 44
#define TNE 45

#define TPLUS 50
#define TMINUS 51
#define TMULT 52
#define TDIV 53
#define TREMAINDER 54
#define TSHOUT 55

#define TANDAND 56
#define TOROR 57

#define TLPAREN 60
#define TRPAREN 61
#define TSEMICOLON 62

#define TMODULE 70
#define TIMPORT 71
#define TEXPORT 72

// Probably going to ignore all this
#define TLINE 80
#define TERROR 81
#define TPRAGMA 82

#define TBADCHAR 99

#ifdef __cplusplus
extern "C" {
#endif

//extern const char* token_id_to_str(uint16_t token_id);

#ifdef __cplusplus
namespace giraffe
{
constexpr uint8_t MAX_INLINE_STR_DELIM = 99;

std::string_view token_id_to_str(uint8_t token_id) noexcept;
const std::vector<uint8_t>& all_token_ids() noexcept;

constexpr bool token_id_is_none(auto id) { return id == TNONE; }
constexpr bool token_id_is_eof(auto id) { return id == TEOF; }
constexpr bool token_id_is_keyword(auto id)
{
   return id >= TIF && id <= TUNDEF;
}
constexpr bool token_id_is_newline(auto id)
{
   return id >= TNEWLINE && id <= TIN_COMMENT_NEWLINE;
}
constexpr bool token_id_is_badchar(auto id) { return id == TBADCHAR; }
constexpr bool token_id_is_comment(auto id)
{
   return id >= TIN_COMMENT_NEWLINE && id <= TCOMMENT_PART;
}
constexpr bool token_id_is_whitespace(auto id)
{
return id == TWHITESPACE;
}
constexpr bool token_id_can_by_zero_length(auto id)
{
   return id == TEOF || id == TSTART;
}


} // namespace giraffe
#endif

#ifdef __cplusplus
}
#endif
