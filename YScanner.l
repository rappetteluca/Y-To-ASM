Letter	[A-Za-z_]
Digit   [0-9]
Symbol  [\\]?[!-&(-~\-]
Integer [-]?([0-9])+

%{
/* Author:      Lucas Rappette
   Created:     04/02.18
   Resources:   https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.2.0/com.ibm.zos.v2r2.bpxa600/genyac.htm
*/
#include "IOMngr.h"
#define YY_INPUT(buf,result,max_size) \
{ int c = GetSourceChar(); \
	result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
}

bool shouldLog = false;

#define RETURN(val) \
{ if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n",val,yyleng,yytext); \
  return(val); \
}

#include "YSemantics.h"
#include "y.tab.h"

int commentLevel = 0;
int lineComment = 0;
void opencomment();
void closecomment();
#define IN_COMMENT (lineComment || commentLevel > 0)

%}

%%
"/*"														{ opencomment(); }
"*/"														{ closecomment(); }
"//"														{ lineComment = 1; }

"while"                         { if (!IN_COMMENT) RETURN(WHILE_TOK); }
"cnst"                          { if (!IN_COMMENT) RETURN(CNST_TOK); }
"loop"                          { if (!IN_COMMENT) RETURN(LOOP_TOK); }
"break"                         { if (!IN_COMMENT) RETURN (BREAK_TOK); }
"for"                           { if (!IN_COMMENT) RETURN(FOR_TOK); }
"decl"                          { if (!IN_COMMENT) RETURN(DECL_TOK); }
"impl"                          { if (!IN_COMMENT) RETURN(IMPL_TOK); }
"int"                           { if (!IN_COMMENT) RETURN(INT_TOK); }
"chr"                           { if (!IN_COMMENT) RETURN(CHR_TOK); }
"get"                           { if (!IN_COMMENT) RETURN(GET_TOK); }
"put"                           { if (!IN_COMMENT) RETURN(PUT_TOK); }
"if"                            { if (!IN_COMMENT) RETURN(IF_TOK) ; }
"else"                          { if (!IN_COMMENT) RETURN(ELSE_TOK) ; }
\'{Symbol}\'                    {	if (!IN_COMMENT) RETURN(CHARLIT_TOK); }
{Letter}?\"(\\.|[^\\"])*\"      { if (!IN_COMMENT) RETURN(STRLIT_TOK); }
"<="			                      { if (!IN_COMMENT) RETURN(LE_TOK); }
">="			                      { if (!IN_COMMENT) RETURN(GE_TOK); }
"=="		                        { if (!IN_COMMENT) RETURN(EQ_TOK); }
"##"		                       	{ if (!IN_COMMENT) RETURN(NE_TOK); }
"&&"			                      { if (!IN_COMMENT) RETURN(AND_TOK); }
"||"		                       	{ if (!IN_COMMENT) RETURN(OR_TOK); }
"<"			                        { if (!IN_COMMENT) RETURN(LT_TOK); }
">"			                        { if (!IN_COMMENT) RETURN(GT_TOK); }
"!"                             { if (!IN_COMMENT) RETURN(NOT_TOK); }
"&"                             { if (!IN_COMMENT) RETURN('&'); }
"@"                             { if (!IN_COMMENT) RETURN('@'); }
"|"                             { if (!IN_COMMENT) RETURN('|'); }
":"                             { if (!IN_COMMENT) RETURN(':'); }
";"                             { if (!IN_COMMENT) RETURN(';'); }
"("                             { if (!IN_COMMENT) RETURN('('); }
")"                             { if (!IN_COMMENT) RETURN(')'); }
"="														  { if (!IN_COMMENT) RETURN('='); }
"-"                             { if (!IN_COMMENT) RETURN('-'); }
"+"                             { if (!IN_COMMENT) RETURN('+'); }
"*"                             { if (!IN_COMMENT) RETURN('*'); }
"/"                             { if (!IN_COMMENT) RETURN('/'); }
"{"                             { if (!IN_COMMENT) RETURN('{'); }
"}"                             { if (!IN_COMMENT) RETURN('}'); }
","                             { if (!IN_COMMENT) RETURN(','); }
{Integer}												{	if (!IN_COMMENT) RETURN(INTLIT_TOK); }
{Letter}({Letter}|{Digit})*     { if (!IN_COMMENT) RETURN(IDENT_TOK); }

[ \t]                           { }
[\n]                            { lineComment = 0; }
.                               { }
%%

void opencomment()
{
		commentLevel += 1;
}

void closecomment()
{
		commentLevel -= 1;
}
