
#pragma once

#include <stdint.h>

#define TNONE 0

/* occurs at the start of every (new) file */
#define TSTART 1

/* occurs ONCE after all inputs have been processed */
#define TEOF 2

/* ----------------------------- Keywords */
#define TIF 10
#define TIFDEF 11
#define TIFNDEF 12
#define TELIF 13
#define TELSE 14
#define TENDIF 15

#define TINCLUDE 20
#define TDEFINE 21
#define TUNDEF 22

/* ----------------------------- Stuff */

#define TIDENTIFIER 30
#define TSTRING 31
#define TSTRINGIFY 32
#define TDIGIT 33

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

#define TNEWLINE 70
#define TNEWLINE_ESC 71
#define TWHITESPACE 72
#define TCOMMENT 73
#define TCOMMENT_DELIM 74
#define TCOMMENT_END_DELIM 75
#define TIN_COMMENT_NEWLINE 76
#define TCOMMENT_PART 77

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
