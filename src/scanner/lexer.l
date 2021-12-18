%{
#include <stdio.h>
#include <assert.h>
#include "scanner/lex-bridge.h"

#define YY_INPUT(buf, result, max_size) {                                   \
        result = worker_read_input(yyextra->scanner_worker, buf, max_size); \
        if(result == 0) result == YY_NULL;                                  \
    }

#define TOKEN(TOK_ID)  (lexx_token((TOK_ID), yyextra, yytext, yyleng))

static int lexx_token(int token_id,
                      struct lex_params_t* params,
                      const char* text,
                      int text_len);

static void push_lex_state(struct lex_params_t* params, int state);
static void pop_lex_state(struct lex_params_t* params);

#define PUSH_STATE(val) { push_lex_state(yyextra, (val)); }
#define POP_STATE()     { pop_lex_state(yyextra); }
#define CURRENT_STATE() (yyextra->lex_state)
#define IS_HEREDOC_DELIM() \
     (CURRENT_STATE() == HEREDOC && test_heredoc_delim(yyextra, yytext))
#define INC_PAREN()       yyextra->stack->paren_count++
#define DEC_PAREN()       decrement_paren_count(yyextra)
%}

%option 8bit noyywrap reentrant
%option extra-type="struct lex_params_t *"
%option stack
%option warn nodefault
%option always-interactive

NL      ((\r\n)|(\n\r)|([\r\n]))
WS      (([\t ])|(\\{NL}))
L	[@_a-zA-Z]
D	[@_\-\./\\a-zA-Z0-9]
ML      [_a-zA-Z]
MD      [_\x2ea-zA-Z0-9]
DIGIT   [0-9]
A       ([\x01-\x09])|([\x20-\x7f])|{WS}|{NL}
NQ      ([\x21-\x21])|([\x23-\x7f])|{WS}
NA      ([\x21-\x3d])|([\x3f-\x7f])|{WS}

PP      ^{WS}*#{WS}*

%x MLC

%%

<<EOF>>			       { return TOKEN(TEOF); }

%{ /* ----------------------------------------------------------- Comments */ %}
"/*"		{ PUSH_STATE(MLC); return TOKEN(TCOMMENT_DELIM); }
<MLC>{
"*/"            { POP_STATE(); return TOKEN(TCOMMENT_END_DELIM); }
{NL}            { return TOKEN(TIN_COMMENT_NEWLINE); }
[^*\n]+         { return TOKEN(TCOMMENT_PART); }
"*"             { return TOKEN(TCOMMENT_PART); }
}

%{ /* ------------------------------------------------------- Preprocessor */ %}

{PP}"pragma"                   { return TOKEN(TPRAGMA); }
{PP}"line"                     { return TOKEN(TPRAGMA); }
{PP}"error"                    { return TOKEN(TPRAGMA); }
{PP}"warn"                     { return TOKEN(TPRAGMA); }

{PP}"ifdef"                    { return TOKEN(TIFDEF); }
{PP}"ifndef"                   { return TOKEN(TIFNDEF); }
{PP}"if"                       { return TOKEN(TIF); }
{PP}"elif"                     { return TOKEN(TELIF); }
{PP}"else"                     { return TOKEN(TELSE); }
{PP}"endif"                    { return TOKEN(TENDIF); }

{PP}"include"                  { return TOKEN(TINCLUDE); }
{PP}"define"                   { return TOKEN(TDEFINE); }
{PP}"undef"                    { return TOKEN(TUNDEF); }

"\""{NQ}*"\""	               { return TOKEN(TSTRING); }
"<"{NA}*">"	               { return TOKEN(TSTRING); }

"import"                       { return TOKEN(TIMPORT); }
"export"                       { return TOKEN(TEXPORT); }
"module"                       { return TOKEN(TMODULE); }

{L}{D}*                        { return TOKEN(TIDENTIFIER); }
{DIGIT}*                       { return TOKEN(TDIGIT); }
{ML}{MD}*                      { return TOKEN(TMODULENAME); }

"##"                           { return TOKEN(TSTRINGIFY); }

"//".*			       { return TOKEN(TCOMMENT); }

"("                            { return TOKEN(TLPAREN); }
")"                            { return TOKEN(TRPAREN); }
"<="                           { return TOKEN(TLE); }
">="                           { return TOKEN(TGE); }
"<"                            { return TOKEN(TLT); }
">"                            { return TOKEN(TGT); }
"=="                           { return TOKEN(TEQEQ); }
"!="                           { return TOKEN(TNE); }
"+"                            { return TOKEN(TPLUS); }
"-"                            { return TOKEN(TMINUS); }
"*"                            { return TOKEN(TMULT); }
"/"                            { return TOKEN(TDIV); }
"%"                            { return TOKEN(TREMAINDER); }
"!"                            { return TOKEN(TSHOUT); }
"&&"                           { return TOKEN(TANDAND); }
"||"                           { return TOKEN(TOROR); }
";"                            { return TOKEN(TSEMICOLON); }

"\\"{WS}*{NL}                  { return TOKEN(TNEWLINE_ESC); }
{WS}+			       { return TOKEN(TWHITESPACE); }
{NL}                           { return TOKEN(TNEWLINE); }

[\x00-\xff]		       { TOKEN(TBADCHAR); }

%%



static void lex_params_init(struct lex_params_t* params,
                            void * lex,
                            void * scanner_worker)
{
    memset(params, 0, sizeof(struct lex_params_t));
    params->lex = lex;
    params->scanner_worker = scanner_worker;
}

static void pop_lex_state(struct lex_params_t* params)
{
    params->n_concurrent_states--;
    params->lex_state = yy_top_state(params->lex);
    yy_pop_state(params->lex);
}

static void push_lex_state(struct lex_params_t* params, int state)
{
    params->n_concurrent_states++;
    params->lex_state = state;
    yy_push_state(state, params->lex);
}

static void lex_params_destroy(struct lex_params_t* params)
{
    free(params);
}

extern yyscan_t init_lex(void* scanner_worker)
{
    struct lex_params_t* params = malloc(sizeof(struct lex_params_t));
    yyscan_t lex;

    yylex_init(&lex);
    lex_params_init(params, lex, scanner_worker);    
    yyset_extra(params, lex);
    
    return lex;
}

extern void destroy_lex(yyscan_t lex)
{
    lex_params_destroy(yyget_extra(lex));
    yylex_destroy(lex);
}

extern struct lex_params_t* get_lex_params(yyscan_t lex)
{
    return yyget_extra(lex);
}

static void update_token(int token_id,
                         struct lex_params_t* params,
                         const char * text,
                         int text_len
                         )
{
    params->text_len += text_len;
    params->offset += text_len;
    params->column_no += text_len;

    if(token_id == TNEWLINE) {
        const char* pos = text;
        while(*pos) if(*pos++ == '\n') params->line_no += 1;
        params->column_no = 0;
    } else if(token_id == TNEWLINE_ESC) {
        params->column_no = 0;
        params->line_no += 1;
    }
}

static int start_token(int token_id,
                       struct lex_params_t* params,
                       const char* text,
                       int text_len)
{
    params->token_id = token_id;
    assert(params->token_id == token_id);

    params->text = text;
    params->text_len = 0;
    params->last_line_no = params->line_no;
    params->last_column_no = params->column_no;
    params->last_offset = params->offset;

    update_token(token_id, params, text, text_len);

    return token_id;
}

static int lexx_token(int token_id,
                      struct lex_params_t* params,
                      const char* text,
                      int text_len)
{
    // printf("\nH tok=%03d 0x%02x\n", token_id, (text)[0]);
    return start_token(token_id, params, text, text_len);
}
