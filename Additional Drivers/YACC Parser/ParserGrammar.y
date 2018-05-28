%{
/* Author:      Lucas Rappette
   Created:     04/02.18
   Resources:   https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.2.0/com.ibm.zos.v2r2.bpxa600/genyac.htm
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IOMngr.h"

extern int yylex(); /* The next token function. */
extern char *yytext; /* The matched token text. */
extern int yyleng; /* The token text length. */

void yyerror(char *s);

#define YYSTYPE long /* 64 bit so can hold pointer and int */

%}

// These token definitions populate y.tab.h
// Single character tokens are their own token numbers (e.g. scanner returns
// the value ';' for the semicolon token)

%token INT_TOK 1
%token CHR_TOK 2
%token ASSIGN_TOK 3
%token INTLIT_TOK 4
%token IDENT_TOK 5

%%
Prog    : IDENT_TOK '{' StmtSeq '}'                                               ;

StmtSeq : Stmt ';' StmtSeq                                                        ;
StmtSeq :                                                                         ;

Stmt    : Decl
        | Assign                                                                  ;

Decl    : Type IDLst                                                              ;

Type    : INT_TOK
        | CHR_TOK                                                                 ;

IDLst   : IDENT_TOK MLst                                                          ;

MLst    : ',' IDLst                                                               ;
MLst    :                                                                         ;

Assign  : LHS ASSIGN_TOK Expr                   { printf("%s =\n",(char *)$1); }  ;

LHS     : IDENT_TOK                              { $$ = (long) strdup(yytext); }  ;

Expr    : Term MExpr                                                              ;

MExpr   : AddOp Term MExpr                         { printf("%s ",(char *)$1); }  ;
MExpr   :                                                                         ;

Term    : Factor MTerm                                                            ;

MTerm   : MultOp Factor MTerm                      { printf("%s ",(char *)$1); }  ;
MTerm   :                                                                         ;

Factor  : '(' Expr ')'
        | INTLIT_TOK                 { printf("%s ", (char *) strdup(yytext)); }  ;
        | IDENT_TOK                  { printf("%s ", (char *) strdup(yytext)); }  ;

AddOp   : '-'                                    { $$ = (long) strdup(yytext); }
        | '+'                                    { $$ = (long) strdup(yytext); }  ;

MultOp  : '*'                                    { $$ = (long) strdup(yytext); }
        | '/'                                    { $$ = (long) strdup(yytext); }  ;
%%

void
yyerror(char *s) {
 // Called when parser encounters error. Write newline to stdout in case partial
 // postfix expression has been generated. Format error message and post to
 // current line. IOMngr will then echo error line along with error message.
 char msg[256];
 printf("\n");
 snprintf(msg,255,"error: \"%s\" yytext: \"%s\"",s,yytext);
 PostMessage(GetCurrentColumn(),yyleng,msg);
}
