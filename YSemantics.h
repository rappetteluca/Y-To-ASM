/* Semantics.h
 The action and supporting routines for performing semantics processing.
 */

#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"
#include "YCodeGen.h"

/* Declaration of semantic record data types used in grammar.y %union */
struct IdList {
  struct SymEntry * entry;
  struct IdList * next;
} typedef IdList_t;

enum BaseTypes { IntBaseType, ChrBaseType, StrLitBaseType };
enum Comparisons { LtCmp, LteCmp, GtCmp, GteCmp, EqCmp, NeqCmp };
enum Operators { addOP, subOP, mulOP, divOP, negOP };
enum BoolOperators { andOP, orOP, notOP };

struct FuncDesc {
  enum BaseTypes returnType;
  struct InstrSeq * funcCode;
} typedef FuncDesc_t;


enum DeclTypes { PrimType, FuncType, ConstantType };
struct TypeDesc {
  enum DeclTypes declType;
  union {
    enum BaseTypes primDesc;
    struct FuncDesc * funcDesc;
    int constantVal;
  };
} typedef TypeDesc_t;

// Symbol Table Structures
struct Attr {
  struct TypeDesc * typeDesc;
  char * reference;
} typedef Attr_t;

struct ExprResult {
  struct InstrSeq * exprCode;
  enum BaseTypes resultType;
  union {
    int resultRegister;
    char * resultLabel;
  };
} typedef ExprResult_t;

struct CondResult {
  struct InstrSeq * branchCode;
  char * branchLabel;
  enum Comparisons branchType;
} typedef CondResult_t;
// Supporting Routines
void InitSemantics();
void ListIdentifierTable();
void processStringLiterals(struct SymEntry * entry, int cnt, void * dataCode);

struct TypeDesc * MakePrimDesc(enum BaseTypes type);
struct TypeDesc * MakeFuncDesc(enum BaseTypes returnType);

// Semantics Actions
void                    Finish();

void                    ProcDecls(struct IdList * idList, enum BaseTypes baseType);
struct IdList *         AppendIdList(struct IdList * item, struct IdList * list);
struct IdList *         ProcName(char * id, enum DeclTypes type);
void                    ProcFunc(char * id, struct InstrSeq * instrs);
void                    ParseError(char *Message, char* id, char* info);
struct InstrSeq *       ProcPut(struct ExprResult * expr);
struct InstrSeq *       ProcAssign(char * id, struct ExprResult * expr);
struct ExprResult *     ProcExpr(struct ExprResult * expr1, enum Operators opcode, struct ExprResult * expr2);
struct ExprResult *     MakeExprIntLit(char * val);
struct ExprResult *     MakeExprCharLit(char * val);
struct ExprResult *     MakeExprIdent(char * ident);
struct ExprResult *     MakeExprStrLit(char * ident);
struct ExprResult *     MakeExprNegative(struct ExprResult * expr);
struct ExprResult *     ProcGet(enum BaseTypes type);
struct ExprResult *     MakePutExpr(struct ExprResult * expr);
struct InstrSeq *
CreateIfThenElseBlock(struct CondResult * ifBlock, struct InstrSeq * thenBlock, struct InstrSeq * elseBlock);
struct CondResult *
CreateCondStatement(enum Comparisons bc, struct ExprResult * left, struct ExprResult * right);
struct InstrSeq *
CreateWhileLoop(struct CondResult * condBlock, struct InstrSeq * codeBlock);
struct InstrSeq *
StepValue(enum Operators op1, enum Operators op2, char * id);
struct CondResult *
CombineConditionals(enum BoolOperators bc, struct CondResult *, struct CondResult *);
struct InstrSeq *
CreateForLoop (struct InstrSeq * assignStmt, struct CondResult * condBlock, struct InstrSeq * codeBlock);
struct InstrSeq *
IndefiniteLoop(struct InstrSeq * codeBlock);
void
ProcCnst(char * id, char * val);
