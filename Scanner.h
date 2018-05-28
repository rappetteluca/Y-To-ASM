/* Scanner.h
   This is really a convenience wrapper around the function and variables
   supplied by lex.
*/

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

extern int yylex();                   // The next token function.
extern char *yytext;                  // The matched token text.
//extern unsigned long yyleng;          // The token text length.
extern size_t yyleng;          // The token text length.


#endif
