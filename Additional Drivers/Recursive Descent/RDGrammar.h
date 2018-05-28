/* RDGrammar.h
     Contains the grammar parsing routines for the grammar

EPS: { StmtSeq, MLst, MExpr, MTerm }
                                          Select Set        Follow Set (Not complete)
<Prog>    :==  Ident { <StmtSeq> }        Ident INT CHAR }           $

<StmtSeq> :==  <Stmt> ; <StmtSeq>         INT CHR Ident              }
<StmtSeq> :==                             }

<Stmt>    :==  <Decl>                     INT CHAR
<Stmt>    :==  <Assign>                   Ident

<Decl>    :==  <Type> <IDLst>             INT CHR

<Type>    :==  INT                        INT
<Type>    :==  CHR                        CHR

<IDLst>   :==  Ident <MLst>               Ident

<MLst>    :==  , <IDLst>                  ,                      Ident
<MLst>    :==                             Ident ;

<Assign>  :==  Ident := <Expr>            Ident

<Expr>    :==  <Term> <MExpr>             ( - IntLit FloatLit Ident

<MExpr>   :==  <AddOp> <Term> <MExpr>     - +
<MExpr>   :==                             ; )

<Term>    :==  <Factor> <MTerm>           ( - IntLit FloatLit Ident

<MTerm>   :==  <MultOp> <Factor> <MTerm>  * /
<MTerm>   :==                             - + ; )

<Factor>  :==  ( <Expr> )                 (
<Factor>  :==  - <Factor>                 -
<Factor>  :==  <IntLit>                   ""
<Factor>  :==  <FloatLit>                 ""
<Factor>  :==  <Ident>                    ""

<AddOp>   :==  -                          -
<AddOp>   :==  +                          +

<MultOp>  :==  *                          *
<MultOp>  :==  /                          /

*/

#include "RDSupport.h"

/* Nonterminal Functions */
bool Prog();
bool StmtSeq();
bool Stmt();
bool Decl();
bool Type();
bool IDLst();
bool MLst();
bool Assign();
bool Expr();
bool MExpr();
bool Term();
bool MTerm();
bool Factor();
bool AddOp();
bool MultOp();
