Letter	[A-Za-z]
Digit   [0-9]
Integer [-]?([0-9])+
Float		[-]?([0-9])+[\.]([0-9])*


%{
/* Author:      Lucas Rappette
   Created:     03/18.18
   Resources:   https://www.usna.edu/Users/cs/roche/courses/f11si413/c10/ff.pdf
*/
#include "RDTokens.h"
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

int comment_depth = 0;
bool IN_LINE_COMMENT = false;
void opencomment();
void closecomment();
#define IN_COMMENT ( ((IN_LINE_COMMENT) || (comment_depth > 0)) ? true : false )

// It should be noted that in gcc numeric values like "-000000000", "-000000001",
// "-0.", and "-00000.0" are all ACCEPTED by the GNU C compiler.
// The leading zeros are trimmed.
// I am going to follow this standard. Although it is poor.

//EOF always means EOF, in a comment block or not.
%}

%%
<<EOF>>                         { RETURN(EOF_TOK); }
"/*"														{ opencomment(); }
"*/"														{ closecomment(); }
"//"														{ IN_LINE_COMMENT = true; }

"int"                           { if (!IN_COMMENT) RETURN(INT_TOK); }
"char"                          { if (!IN_COMMENT) RETURN(CHR_TOK); }
":="														{ if (!IN_COMMENT) RETURN(ASSIGN_TOK); }
";"                             { if (!IN_COMMENT) RETURN(SEMI_TOK); }
"("                             { if (!IN_COMMENT) RETURN(LPAREN_TOK); }
")"                             { if (!IN_COMMENT) RETURN(RPAREN_TOK); }
"-"                             { if (!IN_COMMENT) RETURN(MINUS_TOK); }
"+"                             { if (!IN_COMMENT) RETURN(PLUS_TOK); }
"*"                             { if (!IN_COMMENT) RETURN(TIMES_TOK); }
"/"                             { if (!IN_COMMENT) RETURN(DIV_TOK); }
"{"                             { if (!IN_COMMENT) RETURN(LBRACE_TOK); }
"}"                             { if (!IN_COMMENT) RETURN(RBRACE_TOK); }
","                             { if (!IN_COMMENT) RETURN(COMMA_TOK); }
{Integer}												{	if (!IN_COMMENT) RETURN(INTLIT_TOK); }
{Float}		 											{	if (!IN_COMMENT) RETURN(FLOATLIT_TOK); }
{Letter}({Letter}|{Digit})*     { if (!IN_COMMENT) RETURN(IDENT_TOK); }
[ \t]                           { }
[\n]                            { if (IN_LINE_COMMENT) IN_LINE_COMMENT = false; }
.                               { }
%%

void opencomment()
{
		comment_depth += 1;
}

void closecomment()
{
		comment_depth -= 1;
}
