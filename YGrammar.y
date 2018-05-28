%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "IOMngr.h"
  #include "Scanner.h"
  #include "YSemantics.h"

  void yyerror(char *s);
  %}

/* Union structure of data that can be attached to non-terminals   */
%union     {
// real-type union-field-name
  int Integer;
  char * Text;
  struct IdList * IdList;
  enum BaseTypes BaseType;
  struct InstrSeq * InstrSeq;
  struct ExprResult * ExprResult;
  struct CondResult * CondResult;
  enum Operators Operators;
  enum Comparisons Conditions;
  enum BoolOperators BoolOperators;
}

/* Type declaration for data attached to non-terminals. Allows     */
/* $# notation to be correctly type casted for function arguments. */
//    <union-field-name> non-terminal
%type <Text> Id
%type <IdList> DeclList
%type <IdList> DeclItem
%type <InstrSeq> DeclImpls
%type <InstrSeq> Cnst
%type <BaseType> Type
%type <Text> CnstType
%type <InstrSeq> FuncBody
%type <InstrSeq> FuncStmts
%type <InstrSeq> Stmt
%type <InstrSeq> AssignStmt
%type <InstrSeq> PutStmt
%type <InstrSeq> WhileStmt
%type <InstrSeq> ForStmt
%type <InstrSeq> StepStmt
%type <InstrSeq> LoopStmt
%type <InstrSeq> IfStmt
%type <InstrSeq> CodeBlk
%type <InstrSeq> ElseClause
%type <ExprResult> Expr
%type <ExprResult> Term
%type <ExprResult> Factor
%type <ExprResult> PutArgType
%type <BaseType> GetStmt
%type <BaseType> GetArgType
%type <CondResult> Comp
%type <CondResult> Cond
%type <Operators> AddOp
%type <Operators> MultOp
%type <Operators> NegOp
%type <Conditions> CondOp
%type <BoolOperators> BoolOp
/* List of token name and corresponding numbers */
/* y.tab.h will be generated from these */
%token IDENT_TOK  	1
%token DECL_TOK   	2
%token IMPL_TOK   	3
%token INT_TOK      4
%token CHR_TOK      5
%token INTLIT_TOK   6
%token CHARLIT_TOK  7
%token GET_TOK      8
%token PUT_TOK      9
%token IF_TOK       10
%token ELSE_TOK     11
%token LE_TOK       12
%token GE_TOK       13
%token EQ_TOK       14
%token NE_TOK       15
%token LT_TOK       16
%token GT_TOK       17
%token WHILE_TOK    18
%token STRLIT_TOK   19
%token AND_TOK      20
%token OR_TOK       21
%token NOT_TOK      22
%token FOR_TOK      23
%token LOOP_TOK     24
%token BREAK_TOK    25
%token CNST_TOK
// can't go past 32 without conflicting with single char tokens
// could use larger token numbers


%%

Module        : DeclImpls                                       { Finish(); };

DeclImpls     : Decl DeclImpls                                  { };
DeclImpls     : Impl DeclImpls                                  { };
DeclImpls     : Cnst DeclImpls                                  { };
DeclImpls     :                                                 { };

Cnst          : CNST_TOK Id ':' CnstType ';'                    { ProcCnst($2, $4); };
CnstType      : INTLIT_TOK                                      { $$ = strdup(yytext); };
CnstType      : CHARLIT_TOK                                     { $$ = strdup(yytext); };

Decl          : DECL_TOK DeclList ':' Type  ';'                 { ProcDecls($2,$4); };
DeclList      : DeclItem ',' DeclList                           { $$ = AppendIdList($1,$3); };
DeclList      : DeclItem                                        { $$ = $1; };

DeclItem      : Id                                              { $$ = ProcName($1,PrimType); };
DeclItem      : Id FuncArgTypes                                 { $$ = ProcName($1,FuncType); };

Id            : IDENT_TOK                                       { $$ = strdup(yytext); };
FuncArgTypes  : '(' ')'                                         {  };

Type          : INT_TOK                                         { $$ = IntBaseType; };
Type          : CHR_TOK                                         { $$ = ChrBaseType; };

Impl          : IMPL_TOK Id FuncArgNames FuncBody ';'           { ProcFunc($2,$4); };
FuncArgNames  : '(' ')'                                         {  };
FuncBody      : '{' FuncStmts '}'                               { $$ = $2; };

FuncStmts     : Stmt ';' FuncStmts                              { $$ = AppendSeq($1, $3); };
FuncStmts     :                                                 { $$ = NULL; };

Stmt          : AssignStmt                                      { $$ = $1; };
Stmt          : PutStmt                                         { $$ = $1; };
Stmt          : WhileStmt                                       { $$ = $1; };
Stmt          : IfStmt                                          { $$ = $1; };
Stmt          : StepStmt                                        { $$ = $1; };
Stmt          : ForStmt                                         { $$ = $1; };
Stmt          : LoopStmt                                        { $$ = $1; };
Stmt          : BREAK_TOK                                       { $$ = GenInstr(NULL, "break", NULL, NULL, NULL); };

AssignStmt    : Id '=' Expr                                     { $$ = ProcAssign($1, $3); };

PutStmt       : PUT_TOK '(' PutArgType ')'                         { $$ = ProcPut($3); };
PutArgType    : STRLIT_TOK                                      { $$ = MakeExprStrLit(strdup(yytext)); };
PutArgType    : Expr                                            { $$ = $1; };

ForStmt       : FOR_TOK '(' AssignStmt ';' Comp ';' StepStmt ')' CodeBlk { $$ = CreateForLoop($3, $5, AppendSeq($9, $7)); };

WhileStmt     : WHILE_TOK '(' Comp ')' CodeBlk                  { $$ = CreateWhileLoop($3, $5); };

LoopStmt      : LOOP_TOK CodeBlk                                { $$ = IndefiniteLoop($2); };

IfStmt        : IF_TOK '(' Comp ')' CodeBlk ElseClause          { $$ = CreateIfThenElseBlock($3, $5, $6); };

StepStmt      : AddOp AddOp Id                                  { $$ = StepValue($1, $2, $3); };
ElseClause    : ELSE_TOK CodeBlk                                { $$ = $2;  };
ElseClause    :                                                 { $$ = NULL; };

Comp          : Cond BoolOp Comp                                { $$ = CombineConditionals($2, $1, $3); };
Comp          : NOT_TOK '(' Comp ')'                             { $$ = CombineConditionals(notOP, $3, $3); };
Comp          : Cond                                            { $$ = $1; };

Cond          : Expr CondOp Expr                                { $$ = CreateCondStatement($2, $1, $3); };
Cond          : Expr                                            { $$ = CreateCondStatement(NeqCmp, $1, $1); };

CodeBlk       : FuncBody                                        { $$ = $1; };

Expr          : Expr AddOp Term                                 { $$ = ProcExpr($1, $2, $3); };
Expr          : Term                                            { $$ = $1; };

Term          : Term MultOp Factor                              { $$ = ProcExpr($1, $2, $3); };
Term          : Factor                                          { $$ = $1; };

Factor        : '(' Expr ')'                                    { $$ = $2; };
Factor        : INTLIT_TOK                                      { $$ = MakeExprIntLit( strdup(yytext)); };
Factor        : CHARLIT_TOK                                     { $$ = MakeExprCharLit( strdup(yytext)); };
Factor        : Id                                              { $$ = MakeExprIdent($1); };
Factor        : GetStmt                                         { $$ = ProcGet($1); };
Factor        : NegOp Factor                                    { $$ = MakeExprNegative($2); };

GetStmt       : GET_TOK '(' GetArgType                          { $$ = $3; };
GetArgType    : Type ')'                                        { $$ = $1; };

BoolOp        : AND_TOK                                         { $$ = andOP; };
BoolOp        : OR_TOK                                          { $$ = orOP; };

CondOp        : LT_TOK                                             { $$ = LtCmp; };
CondOp        : LE_TOK                                             { $$ = LteCmp; };
CondOp        : GT_TOK                                             { $$ = GtCmp; };
CondOp        : GE_TOK                                             { $$ = GteCmp; };
CondOp        : EQ_TOK                                             { $$ = EqCmp; };
CondOp        : NE_TOK                                             { $$ = NeqCmp; };

AddOp         : '-'                                             { $$ = subOP; };
AddOp         : '+'                                             { $$ = addOP; };

MultOp        : '*'                                             { $$ = mulOP; };
MultOp        : '/'                                             { $$ = divOP; };

NegOp         : '-'                                             { $$ = negOP; };

%%

void
yyerror( char *s)     {
  char msg[MAXLINE];
  sprintf(msg,"ERROR \"%s\" token: \"%s\"",s,yytext);
  PostMessage(GetCurrentColumn(),strlen(yytext),msg);
}
