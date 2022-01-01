
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
#define TELIFDEF 17
#define TELIFNDEF 18
#define TELSE 19
#define TENDIF 20

#define TINCLUDE 21
#define TDEFINE 22
#define TUNDEF 23

/* ----------------------------- Stuff */

#define TIDENTIFIER 30
#define TSTRING 31
#define TSTR_PART 32
#define TSTR_DELIM 33
#define TCSTRING 34
#define TSTRINGIFY 35
#define TINTEGER 36
#define TDEFINED 37

#define TSPACESHIP 40
#define TLT 41
#define TGT 42
#define TLE 43
#define TGE 44
#define TEQEQ 45
#define TNE 46

#define TPLUS 50
#define TMINUS 51
#define TMULT 52
#define TDIV 53
#define TREMAINDER 54
#define TSHOUT 55
#define TTILDE 56

#define TANDAND 57
#define TOROR 58

#define TLPAREN 60
#define TRPAREN 61
#define TSEMICOLON 62
#define TQUESTION 63
#define TCOLON 64
#define TCOMMA 65

#define TAND 70
#define TOR 71
#define TCARROT 72
#define TLTLT 73
#define TGTGT 74

#define TMODULE 170
#define TIMPORT 171
#define TEXPORT 172

// Probably going to ignore all this
#define TLINE 180
#define TERROR 181
#define TWARNING 182

#define TBADCHAR 199

#ifdef __cplusplus
extern "C" {
#endif
extern const char* token_id_to_cstr(uint8_t token_id);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace giraffe
{
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

