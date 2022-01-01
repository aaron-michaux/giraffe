
#pragma once

#include "token-types.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct shell_stack_type
{
   struct shell_stack_type* next; //!< It's a linked list
   char* heredoc_delim;           //!< Parsing HEREDOC strings
   int paren_count;               //!< For when to pop a subshell
};

/**
 * Parameter block that bridges between `flex` and the scanner
 */
struct lex_params_t
{
   const char* text;               //!< Token text (memory owned by lexer)
   uint32_t text_len;              //!< Length of the current token
   uint32_t offset;                //!< Offset into the underlying source file
   uint32_t last_offset;           //!< Offset of previous token
   uint32_t line_no;               //!< Line number of current token, 0-indexed
   uint32_t last_line_no;          //!< Line number of previous token, 0-indexed
   uint16_t column_no;             //!< Column number of curren token, 0-indexed
   uint16_t last_column_no;        //!< Column number of previous token, 0-indexed
   uint8_t token_id;               //!< Token identifier
   int lex_state;                  //!< The current state of the lex
   int n_concurrent_states;        //!< Number of states on the state stack
   struct shell_stack_type* stack; //!< End 'heredoc' delimiter, if any
   void* lex;                      //!< Unmanaged pointer back to the (flex) lex
   void* scanner_worker;           //!< Unmanaged pointer back to (cpp) scanner_worker
};

/** Create (and initialize) a new lex */
extern void* init_lex(void* scanner_worker);

/** Free resources allocated to the lex */
extern void destroy_lex(void* lex);

/** Retrieve lex parameters */
extern struct lex_params_t* get_lex_params(void* lex);

/** Get flex to lex a single token */
extern int yylex(void* lex);

/** Read up to `max_size` bytes into `buffer` using `lex`
 * THIS FUNCTION IS DEFINED IN `scanner.cpp`.
 */
extern size_t worker_read_input(void* worker, char* buffer, size_t max_size);

#ifdef __cplusplus
}
#endif
