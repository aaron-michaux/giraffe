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
WS      ([\t ])
L	[_a-zA-Z]
D	[_a-zA-Z0-9]
ML	[_\.\:a-zA-Z]
MD	[_\.\:a-zA-Z0-9]
DIGIT   [0-9']
HEXD    [0-9a-fA-F']
HEXQUAD {HEXD}{HEXD}{HEXD}{HEXD}
SUFFIX  ([uU])|([zZ])|([lL])
A       ([\x01-\x09])|([\x20-\x7f])|{WS}|{NL}
NQ      ([\x21-\x21])|([\x23-\x7f])|([\t ])
NA      ([\x21-\x3d])|([\x3f-\x7f])|{WS}
NNL     ([^\n\r\\])                                    
DQUOTE  [\x22]
                        
PP      {WS}*#{WS}*
                        
%x MLC
%x PREPROC
%x CSTR
%x MODULE                                        

%%

<<EOF>>			       { return TOKEN(TEOF); }

%{ /* ----------------------------------------------------------- Comments */ %}
"/*"{NNL}*{NL}?                { PUSH_STATE(MLC); yyless(2); return TOKEN(TCOMMENT_DELIM); }
<MLC>{
"*/"                           { POP_STATE(); return TOKEN(TCOMMENT_END_DELIM); }
{NL}                           { return TOKEN(TIN_COMMENT_NEWLINE); }
[^*\n]+                        { return TOKEN(TCOMMENT_PART); }
"*"                            { return TOKEN(TCOMMENT_PART); }
}
"//".*			       { return TOKEN(TCOMMENT); }

%{ /* ------------------------------------------------------- Preprocessor */ %}

{PP}{NNL}*{NL}                 { PUSH_STATE(PREPROC); yyless(0); }
<PREPROC>{

"/*"{NNL}*{NL}                 { PUSH_STATE(MLC); yyless(2); return TOKEN(TCOMMENT_DELIM); }

{PP}"ifdef"                    { return TOKEN(TIFDEF); }
{PP}"ifndef"                   { return TOKEN(TIFNDEF); }
{PP}"if"                       { return TOKEN(TIF); }
{PP}"elif"                     { return TOKEN(TELIF); }
{PP}"elifdef"                  { return TOKEN(TELIFDEF); }
{PP}"elifndef"                 { return TOKEN(TELIFNDEF); }
{PP}"else"                     { return TOKEN(TELSE); }
{PP}"endif"                    { return TOKEN(TENDIF); }

{PP}"include"                  { return TOKEN(TINCLUDE); }
{PP}"define"                   { return TOKEN(TDEFINE); }
{PP}"undef"                    { return TOKEN(TUNDEF); }
{PP}"line"                     { return TOKEN(TLINE); }
{PP}"error"                    { return TOKEN(TERROR); }
{PP}"warning"                  { return TOKEN(TWARNING); }

"\""                           { PUSH_STATE(CSTR); return TOKEN(TSTR_DELIM); }
"<"{NA}*">"	               { return TOKEN(TSTRING); }

{L}{D}*                        { return TOKEN(TIDENTIFIER); }
{DIGIT}+{SUFFIX}*              { return TOKEN(TINTEGER); }
"0x"{HEXD}+{SUFFIX}*           { return TOKEN(TINTEGER); }

"##"                           { return TOKEN(TSTRINGIFY); }
"#"                            { return TOKEN(THASH); }

"("                            { return TOKEN(TLPAREN); }
")"                            { return TOKEN(TRPAREN); }
";"                            { return TOKEN(TSEMICOLON); }
":"                            { return TOKEN(TCOLON); }
","                            { return TOKEN(TCOMMA); }

%{ /* Comparison */ %}
"<="                           { return TOKEN(TLE); }
">="                           { return TOKEN(TGE); }
"<"                            { return TOKEN(TLT); }
">"                            { return TOKEN(TGT); }
"=="                           { return TOKEN(TEQEQ); }
"!="                           { return TOKEN(TNE); }

%{ /* Unary */ %}
"+"                            { return TOKEN(TPLUS); }
"-"                            { return TOKEN(TMINUS); }
"!"                            { return TOKEN(TSHOUT); }
"~"                            { return TOKEN(TTILDE); }

%{ /* Binary (but not unary) */ %}
"*"                            { return TOKEN(TMULT); }
"/"                            { return TOKEN(TDIV); }
"%"                            { return TOKEN(TREMAINDER); }
"&"                            { return TOKEN(TAND); }
"|"                            { return TOKEN(TOR); }
"^"                            { return TOKEN(TCARROT); }
"&&"                           { return TOKEN(TANDAND); }
"||"                           { return TOKEN(TOROR); }
"<<"                           { return TOKEN(TLTLT); }
">>"                           { return TOKEN(TGTGT); }

"\\"{NL}                       { return TOKEN(TNEWLINE_ESC); }
{WS}+			       { return TOKEN(TWHITESPACE); }
{NL}                           { POP_STATE(); return TOKEN(TNEWLINE); }

[\x00-\xff]		       { return TOKEN(TLINE_PART); }
}

<CSTR>{
[^\"\r\n\\]+                   { return TOKEN(TSTR_PART); }
"\\\""                         { return TOKEN(TSTR_PART); }
"\\"[\'\"?abfnrtv\\]           { return TOKEN(TSTR_PART); }
"\\"[0-7]{1,3}                 { return TOKEN(TSTR_PART); }
"\\x"{HEXD}+                   { return TOKEN(TSTR_PART); }
{DQUOTE}                       { POP_STATE(); return TOKEN(TSTR_DELIM); }

[\x00-\xff]		       { TOKEN(TBADCHAR); }
}

"import"({WS}{NNL}*)?          { yyless(6); PUSH_STATE(MODULE); return TOKEN(TIMPORT); }
"export"({WS}{NNL}*)?          { yyless(6); PUSH_STATE(MODULE); return TOKEN(TEXPORT); }
"module"({WS}{NNL}*)?          { yyless(6); PUSH_STATE(MODULE); return TOKEN(TMODULE); }

<MODULE>{
"import"                       { return TOKEN(TIMPORT); }
"export"                       { return TOKEN(TEXPORT); }
"module"                       { return TOKEN(TMODULE); }
{ML}{MD}*                      { return TOKEN(TIDENTIFIER); }
{WS}+			       { return TOKEN(TWHITESPACE); }
";"                            { return TOKEN(TSEMICOLON); }
"\\"{NL}                       { return TOKEN(TNEWLINE_ESC); }
{NL}                           { POP_STATE(); return TOKEN(TNEWLINE); }
[\x00-\xff]		       { return TOKEN(TBADCHAR); }
}

{NNL}+                         { return TOKEN(TLINE_PART); }

"\\"{NL}                       { return TOKEN(TNEWLINE_ESC); }
{NL}                           { return TOKEN(TNEWLINE); }

"\\"                           { return TOKEN(TLINE_PART); }

%%

// [\x00-\xff]		       { TOKEN(TBADCHAR); }

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
