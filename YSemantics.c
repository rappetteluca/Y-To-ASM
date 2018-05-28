/* Semantics.c
 Support and semantic action routines.

 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "YSemantics.h"

// Shared Data

struct SymTab * IdentifierTable;
struct SymTab * StringTable;
struct SymTab * ConstantsTable;
enum AttrKinds { VOID_KIND = -1, INT_KIND, STRING_KIND, STRUCT_KIND };

char * BaseTypeNames[3] = { "int", "chr", "string"};
char * TypeNames[2] = { "", "func"};

// corresponds to enum Operators, last one is for negate
char * Ops[] = { "add", "sub", "mul", "div", "subu"};
char * BoolOps[] = { "and", "or", "not" };

// corresponds to negation of enum Comparisons
char * Branches[] = { "bge", "bg", "ble", "bl", "bne", "beq"};
// comparison set instructions, in order with enum Comparisons
char * CmpSetReg[] = { "slt", "sle", "sgt", "sge", "seq", "sne" };
char * BreakStmt = "break";


// Supporting Routines
void
PostMessageAndExit(int col, char * message) {
  PostMessage(col,1,message);
  CloseSource();
  exit(0);
}
void
ParseError(char *Message, char* id, char* info) {
  char * buf = malloc(strlen(id) + strlen(Message) + strlen(info) + 34 + 1);
	sprintf(buf,"Error: %s -- %s -- %s", info,id,Message);
	PostMessage(GetCurrentColumn(),1,buf);
  free(buf);
  CloseSource();
  exit(1);
}

void
InitSemantics() {
  IdentifierTable = CreateSymTab(100,"global",NULL);
}

char *
StringForType(struct TypeDesc * desc) {
  switch (desc->declType) {
    case PrimType: {
      return strdup(BaseTypeNames[desc->primDesc]);
    } break;
    case FuncType: {
      return strdup(BaseTypeNames[desc->funcDesc->returnType]);
    } break;
  }
}

struct TypeDesc *
MakePrimDesc(enum BaseTypes type) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = PrimType;
  desc->primDesc = type;
  return desc;
}

struct TypeDesc *
MakeFuncDesc(enum BaseTypes returnType) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = FuncType;
  desc->funcDesc = malloc(sizeof(struct FuncDesc));
  desc->funcDesc->returnType = returnType;
  desc->funcDesc->funcCode = NULL;
  return desc;
}

void displayEntry(struct SymEntry * entry, int cnt, void * ignore) {
  char * scope = GetScopePath(GetTable(entry));
  printf("%3d %-20s %-15s",cnt,scope,GetName(entry));
  free(scope);
  int attrKind = GetAttrKind(entry);
  struct Attr * attr = GetAttr(entry);
  switch (attrKind) {
    case VOID_KIND: {
    } break;
    case INT_KIND: {
      if (attr) {
        printf("%-10s",attr->reference);
        printf("     ");
        struct TypeDesc * desc = attr->typeDesc;
        char * typeStr = StringForType(desc);
        printf("%-10s ",typeStr);
        free(typeStr);
      }
      else {
        printf("empty");
      }
    } break;
    case STRING_KIND: {
    } break;
    case STRUCT_KIND: {
      if (attr) {
        printf("%-10s",attr->reference);
        printf("     ");
        struct TypeDesc * desc = attr->typeDesc;
        char * typeStr = StringForType(desc);
        printf("%-10s ",typeStr);
        free(typeStr);
      }
      else {
        printf("empty");
      }
    } break;
  }
  printf("\n");
}

void
ListIdentifierTable() {
  printf("\nIdentifier Symbol Table\n");
  printf("Num Scope                Name           Ref       Kind Type\n");
  InvokeOnEntries(IdentifierTable,true,displayEntry,0,NULL);
}

void
FreeTypeDesc(struct TypeDesc * desc) {
  switch (desc->declType) {
    case PrimType: {
    } break;
    case FuncType: {
      if (desc->funcDesc->funcCode) FreeSeq(desc->funcDesc->funcCode);
      free(desc->funcDesc);
    } break;
  }
  free(desc);
}

// provided to the SymTab for destroying table contents
void
FreeEntryAttr(struct SymEntry * entry) {
  int attrKind = GetAttrKind(entry);
  struct Attr * attr = GetAttr(entry);
  switch (attrKind) {
    case VOID_KIND: {
    } break;
    case INT_KIND: {
    } break;
    case STRING_KIND: {
    } break;
    case STRUCT_KIND: {
      if (attr) {
        if (attr->typeDesc)  FreeTypeDesc(attr->typeDesc);
        if (attr->reference) free(attr->reference);
        free(attr);
      }
    } break;
  }
}

void processGlobalIdentifier(struct SymEntry * entry, int cnt, void * dataCode) {
  struct Attr * attr = GetAttr(entry);
  InstrSeqList_t * global_decls;
  InstrSeqList_t * new_decl = GenInstr(attr->reference,".word",Imm(0),NULL,NULL);
  switch (attr->typeDesc->declType) {
    case PrimType: {
      global_decls = (InstrSeqList_t *) dataCode;
      global_decls = AppendSeq(global_decls, new_decl);
    } break;
    case ConstantType: {

    } break;
    case FuncType: {
      // nothing to do here
    } break;
  }
}

void processConstants(struct SymEntry * entry, int cnt, void * dataCode)
{
  struct Attr * attr = GetAttr(entry);
  InstrSeqList_t * global_decls;
  InstrSeqList_t * new_decl;
  switch (attr->typeDesc->declType) {
    case ConstantType: {
      new_decl = GenInstr(attr->reference,".word",Imm(attr->typeDesc->constantVal),NULL,NULL);
      global_decls = (InstrSeqList_t *) dataCode;
      global_decls = AppendSeq(global_decls, new_decl);
    } break;
    case PrimType: {

    } break;
    case FuncType: {
      // nothing to do here
    } break;
  }
}

void processStringLiterals(struct SymEntry * entry, int cnt, void * dataCode) {
  struct Attr * attr = GetAttr(entry);
  char * val = strdup(GetName(entry));
  InstrSeqList_t * global_decls;
  InstrSeqList_t * new_decl = GenInstr(attr->reference,".asciiz", val,NULL,NULL);
  free(val);
  switch (attr->typeDesc->declType) {
    case PrimType: {
      global_decls = (InstrSeqList_t *) dataCode;
      global_decls = AppendSeq(global_decls, new_decl);
    } break;
    case FuncType: {
      // nothing to do here
    } break;
  }
}

void processFunctions(struct SymEntry * entry, int cnt, void * textCode) {
  struct Attr * attr = GetAttr(entry);
  InstrSeqList_t * global_funcs;
  InstrSeqList_t * new_funct;
  switch (attr->typeDesc->declType) {
    case PrimType: {
      // nothing to do here
    } break;
    case FuncType: {
      if (!attr->typeDesc->funcDesc->funcCode) {
        PostMessageAndExit(GetCurrentColumn(),"function never implemented");
      }
      else
      {
        global_funcs = (InstrSeqList_t *) textCode;
        global_funcs = AppendSeq(global_funcs, attr->typeDesc->funcDesc->funcCode);
      }

    } break;
  }
}

// Semantics Actions

void
Finish() {
  printf("Finish\n");
  ListIdentifierTable();

  struct InstrSeq * textCode = GenInstr(NULL,".text",NULL,NULL,NULL);
  struct InstrSeq * dataCode = GenInstr(NULL,".data",NULL,NULL,NULL);

  // form module preamble
  struct SymEntry * mainEntry = LookupName(IdentifierTable,"main");
  if (!mainEntry) {
    PostMessageAndExit(GetCurrentColumn(),"no main function for module");
  }
  // should make sure main takes no arguments
  struct Attr * mainAttr = GetAttr(mainEntry);
  // need to keep spim happy
  AppendSeq(textCode,GenInstr(NULL,".globl","__start",NULL,NULL));
  AppendSeq(textCode,GenInstr("__start",NULL,NULL,NULL,NULL));
  AppendSeq(textCode,GenInstr(NULL,"jal",mainAttr->reference,NULL,NULL));
  AppendSeq(textCode,GenInstr(NULL,"li","$v0","10",NULL));
  AppendSeq(textCode,GenInstr(NULL,"syscall",NULL,NULL,NULL));

  // run SymTab with InvokeOnEntries putting globals in data seg
  InvokeOnEntries(IdentifierTable, true, processGlobalIdentifier, 0, dataCode);
  if(StringTable)
  {
    AppendSeq(dataCode,GenInstr("__iobuf",".space","4",NULL,NULL));
    InvokeOnEntries(StringTable, true, processStringLiterals, 0, dataCode);
  }
  if(ConstantsTable)
  {
    InvokeOnEntries(ConstantsTable, false, processConstants, 0, dataCode);
  }
  // run SymTab with InvokeOnEntries putting functions in code seq
  InvokeOnEntries(IdentifierTable, true, processFunctions, 0, textCode);
  // combine and write
  struct InstrSeq * moduleCode = AppendSeq(textCode,dataCode);
  WriteSeq(moduleCode);

  // free code
  FreeSeq(moduleCode);
  CloseCodeGen();
  fprintf(stderr,"Finish \n");
}
void
ProcCnst(char * id, char * val)
{
  // enter id in symtab
  SymEntry_t * table_entry;
  Attr_t * attr;
  if(!ConstantsTable)
  {
      ConstantsTable = CreateSymTab(100, "constants", IdentifierTable);
  }
  if( !(table_entry = LookupName(ConstantsTable, id)) )
  {
    table_entry = EnterName(ConstantsTable, id);
  // create and partially init type descriptor, completed in ProcDecl
    TypeDesc_t * entry_desc = malloc(sizeof(TypeDesc_t));
    entry_desc->declType = ConstantType;
    if(val[0] == '\'')
    {
      int x = val[1];
      if(x == 92) //The escape character
      {
        switch((char) val[2])
        {
          case 'n':
            x = '\n';
          break;
          case 'e':
            x = '\e';
          break;
          case 't':
            x = '\t';
          break;
        }
      }
      entry_desc->constantVal = x;
    }
    else
    {
      entry_desc->constantVal = atoi(val);
    }
  // create, init and set attr struct
    Attr_t * entry_atr = malloc(sizeof(Attr_t));
    entry_atr->typeDesc = entry_desc;
    entry_atr->reference = GenLabel();
    SetAttr(table_entry, INT_KIND, entry_atr);
  }
}

void
ProcDecls(struct IdList * idList, enum BaseTypes baseType)
{
    IdList_t * pointer = idList;
    Attr_t *  pointer_attrs;
    TypeDesc_t * pointer_desc;
  // walk IdList items
    while(pointer != NULL)
    {
      pointer_attrs = (Attr_t *) GetAttr(pointer->entry);
      pointer_desc = pointer_attrs->typeDesc;
      switch(pointer_desc->declType)
      {
        case PrimType:
            pointer_desc->declType = PrimType;
            pointer_desc->primDesc = baseType;
        break;
        case FuncType:
            pointer_desc->declType = FuncType;
            pointer_desc->funcDesc = malloc(sizeof(struct FuncDesc));
            pointer_desc->funcDesc->returnType = baseType;
            pointer_desc->funcDesc->funcCode = NULL;
        break;
        default:
            ParseError(" : Unknown declaration type", pointer_attrs->reference, "UNK");
        break;
      }
      pointer = pointer->next;

    }
    // switch for prim or func type
      // names on IdList are only specified as PrimType or FuncType
      // set type desc
    // for Sem1 everthing is in global scope, otherwise check scope
    // attr reference string is id name with prepended "_"
}

struct IdList *
AppendIdList(struct IdList * item, struct IdList * list)
{
    item->next = list;
    return item;
}

struct IdList *
ProcName(char * id, enum DeclTypes type)
{
  // get entry for id, error if it exists
  if(LookupName(IdentifierTable, id))
  {
    ParseError("already exists.", id, BaseTypeNames[type]);
    return NULL;
  }
  // enter id in symtab
    SymEntry_t * table_entry = EnterName(IdentifierTable, id);
  // create IdList node for entry
    IdList_t * ident_entry = malloc(sizeof(IdList_t));
    ident_entry->entry = table_entry;
    ident_entry->next = NULL;
  // create and partially init type descriptor, completed in ProcDecl
    TypeDesc_t * entry_desc = malloc(sizeof(TypeDesc_t));
    entry_desc->declType = type;
  // create, init and set attr struct
    char * id_dup = strdup(id);
    Attr_t * entry_atr = malloc(sizeof(Attr_t));
    entry_atr->typeDesc = entry_desc;
    entry_atr->reference = malloc(strlen(id)+1);
    entry_atr->reference[0] = '_';
    entry_atr->reference[1] = '\0';
    entry_atr->reference = strcat(entry_atr->reference, id_dup);
    SetAttr(table_entry, INT_KIND, entry_atr);
    free(id_dup);
    return (ident_entry);
}

void
ProcFunc(char * id, struct InstrSeq * instrs)
{
  // lookup name
  // get attr
  // gen instr for function entry
  // include function body code
  // function exit code, i.e. jump return
  SymEntry_t * entry = LookupName(IdentifierTable, id);
  Attr_t * entry_atr;
  TypeDesc_t * entry_desc;
  if(entry)
  {
    entry_atr = GetAttr(entry);
    entry_desc = entry_atr->typeDesc;
    InstrSeqList_t * seqList = GenInstr(entry_atr->reference,NULL,NULL,NULL,NULL);
    if(instrs)
    {
      seqList = AppendSeq(seqList,instrs);
    }
    seqList = AppendSeq(seqList, GenInstr(NULL, "jr", "$ra", NULL, NULL));
    entry_desc->funcDesc->funcCode = seqList;
  }
  else
  {
    //Error
    PostMessageAndExit(GetCurrentColumn(), "Error: Unknown Entry.");
  }
}
struct InstrSeq * ProcPut(struct ExprResult * expr)
{
  switch(expr->resultType)
  {
    case StrLitBaseType:
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "li", "$v0", "4", NULL));
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "la","$a0",expr->resultLabel, NULL));
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "syscall", NULL, NULL, NULL));
    break;
    case IntBaseType:
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "li", "$v0", "1", NULL));
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "move","$a0",TmpRegName(expr->resultRegister), NULL));
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "syscall", NULL, NULL, NULL));
      break;
    case ChrBaseType:
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "li", "$v0", "11", NULL));
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "move","$a0",TmpRegName(expr->resultRegister), NULL));
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "syscall", NULL, NULL, NULL));
      break;
    default:
      break;
  }
  ReleaseTmpReg(expr->resultRegister);
  //free(expr);
  return expr->exprCode;
}
struct InstrSeq * ProcAssign(char * id, struct ExprResult * expr)
{
  SymEntry_t * table_entry = LookupName(IdentifierTable, id);
  if(table_entry)
  {
    Attr_t * entry_atr = GetAttr(table_entry);
    TypeDesc_t * entry_desc = entry_atr->typeDesc;
    InstrSeqList_t * instructions = expr->exprCode;
    switch(entry_desc->declType)
    {
      case PrimType:
        if(expr->resultType == entry_desc->primDesc)
        {
          instructions = AppendSeq(expr->exprCode, GenInstr(NULL,"sw", TmpRegName(expr->resultRegister),entry_atr->reference,NULL));
          ReleaseTmpReg(expr->resultRegister);
          //free(expr);
          return instructions;
        }
        break;
      case ConstantType:
        if(expr->resultType == IntBaseType)
        {
          instructions = AppendSeq(expr->exprCode, GenInstr(NULL,"sw", TmpRegName(expr->resultRegister),entry_atr->reference,NULL));
          ReleaseTmpReg(expr->resultRegister);
        }
      case FuncType:
        //Nothing to be done
      default:
          ParseError(" : Unknown type assignment", entry_atr->reference, "UNK");
      break;
    }

  }
  return NULL;
}
struct ExprResult * ProcExpr(struct ExprResult * expr1, enum Operators opcode, struct ExprResult * expr2)
{
  if(expr1->resultType == expr2->resultType)
  {
    ExprResult_t * expr = malloc(sizeof(ExprResult_t));
    if(expr)
    {
      expr->resultType = expr1->resultType;
      expr->resultRegister = AvailTmpReg();
      expr->exprCode = AppendSeq(expr->exprCode, expr1->exprCode);
      expr->exprCode = AppendSeq(expr->exprCode, expr2->exprCode);
      expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL,Ops[opcode],TmpRegName(expr->resultRegister),TmpRegName(expr1->resultRegister),TmpRegName(expr2->resultRegister)));
      ReleaseTmpReg(expr1->resultRegister);
      ReleaseTmpReg(expr2->resultRegister);
      return expr;
    }
  }
  return NULL;
}
struct ExprResult * MakeExprIntLit(char * val)
{
  ExprResult_t * expr = malloc(sizeof(ExprResult_t));
  if(expr)
  {
    expr->resultType = IntBaseType;
    expr->resultRegister = AvailTmpReg();
    expr->exprCode = GenInstr(NULL,"li", TmpRegName(expr->resultRegister),val,NULL);
    return expr;
  }
  return NULL;
}
struct ExprResult * MakeExprCharLit(char * val)
{
  ExprResult_t * expr = malloc(sizeof(ExprResult_t));
  if(expr)
  {
    int x = val[1];
    if(x == 92) //The escape character
    {
      switch((char) val[2])
      {
        case 'n':
          x = '\n';
        break;
        case 'e':
          x = '\e';
        break;
        case 't':
          x = '\t';
        break;
      }
    }
    int length = snprintf( NULL, 0, "%d", x );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", x );
    expr->resultType = ChrBaseType;
    expr->resultRegister = AvailTmpReg();
    expr->exprCode = GenInstr(NULL,"li", TmpRegName(expr->resultRegister),str,NULL);
    return expr;
  }
  return NULL;
}
struct ExprResult * MakeExprIdent(char * ident)
{
  SymEntry_t * ident_entry = LookupName(IdentifierTable, ident);
  if(ident_entry)
  {
    Attr_t * entry_atr = GetAttr(ident_entry);
    ExprResult_t * expr = malloc(sizeof(ExprResult_t));
    TypeDesc_t * entry_desc = entry_atr->typeDesc;
    if(expr)
    {
      switch(entry_desc->declType)
      {
        case PrimType:
            expr->resultType = entry_desc->primDesc;
            expr->resultRegister = AvailTmpReg();
            expr->exprCode = GenInstr(NULL,"lw", TmpRegName(expr->resultRegister),entry_atr->reference,NULL);
            return expr;
        break;
        case FuncType:
          expr->resultType = entry_desc->funcDesc->returnType;
          expr->resultRegister = AvailTmpReg();
          expr->exprCode = GenInstr(NULL,"lw", TmpRegName(expr->resultRegister),entry_atr->reference,NULL);
          return expr;
        break;
        default:
            ParseError(" : Unknown declaration type", entry_atr->reference, "UNK");
        break;
      }
    }
  }
  else if ( (ident_entry = LookupName(ConstantsTable, ident)) )
  {
    Attr_t * entry_atr = GetAttr(ident_entry);
    ExprResult_t * expr = malloc(sizeof(ExprResult_t));
    TypeDesc_t * entry_desc = entry_atr->typeDesc;
    expr->resultType = IntBaseType;
    expr->resultRegister = AvailTmpReg();
    expr->exprCode = GenInstr(NULL,"lw", TmpRegName(expr->resultRegister), entry_atr->reference, NULL);
    return expr;
  }
  return NULL;
}
struct ExprResult * MakeExprStrLit(char * ident)
{
  // enter id in symtab
  SymEntry_t * table_entry;
  ExprResult_t * expr;
  Attr_t * attr;
  if(!StringTable)
  {
      StringTable = CreateSymTab(100, "strings", NULL);
  }
  if( (table_entry = LookupName(StringTable, ident)) )
  {
    attr = GetAttr(table_entry);
    expr = malloc(sizeof(ExprResult_t));
    expr->resultType = StrLitBaseType;
    expr->resultLabel = attr->reference;
    expr->exprCode = NULL;
    return expr;
  }
  else
  {
    table_entry = EnterName(StringTable, ident);
  // create and partially init type descriptor, completed in ProcDecl
    TypeDesc_t * entry_desc = malloc(sizeof(TypeDesc_t));
    entry_desc->declType = PrimType;
    entry_desc->primDesc = StrLitBaseType;
  // create, init and set attr struct
    Attr_t * entry_atr = malloc(sizeof(Attr_t));
    entry_atr->typeDesc = entry_desc;
    entry_atr->reference = GenLabel();
    SetAttr(table_entry, STRING_KIND, entry_atr);

    expr = malloc(sizeof(ExprResult_t));
    expr->resultType = StrLitBaseType;
    expr->resultLabel = entry_atr->reference;
    expr->exprCode = NULL;
    return expr;
  }
}
struct ExprResult * MakeExprNegative(struct ExprResult * expr)
{

  if(expr->resultType == IntBaseType)
  {
    InstrSeqList_t * negate = GenInstr(NULL, "subu", TmpRegName(expr->resultRegister), "$0", TmpRegName(expr->resultRegister));
    expr->exprCode = AppendSeq(expr->exprCode, negate);
    return expr;
  }
  return expr;
}
struct ExprResult * ProcGet(enum BaseTypes type)
{
  ExprResult_t * expr = malloc(sizeof(ExprResult_t));
  expr->resultType = type;
  expr->resultRegister = AvailTmpReg();
  switch(type)
  {
    case IntBaseType:
        expr->exprCode = GenInstr(NULL, "li", "$v0", "5", NULL);
    break;
    case ChrBaseType:
        expr->exprCode = GenInstr(NULL, "li", "$v0", "12", NULL);
    break;
  }
  expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "syscall", NULL, NULL, NULL));
  expr->exprCode = AppendSeq(expr->exprCode, GenInstr(NULL, "move", TmpRegName(expr->resultRegister), "$v0", NULL));
  return expr;
}

struct InstrSeq *
CreateIfThenElseBlock(struct CondResult * ifBlock, struct InstrSeq * thenBlock, struct InstrSeq * elseBlock)
{
  char * aLabel;
  if(ifBlock)
  {
    InstrSeqList_t * fullSequence = ifBlock->branchCode;
    if(thenBlock)
    {
      AppendSeq(fullSequence, thenBlock);
      if(elseBlock)
      {
          aLabel = GenLabel();
          AppendSeq(fullSequence, GenInstr(NULL, "b", aLabel, NULL, NULL));
      }
      AppendSeq(fullSequence, GenInstr(ifBlock->branchLabel, NULL, NULL, NULL, NULL));

      if (elseBlock) //else no elseBlock... hehe
      {
        AppendSeq(fullSequence, elseBlock);
        AppendSeq(fullSequence, GenInstr(aLabel, NULL, NULL, NULL, NULL));
      }
    }
    return fullSequence;
  }
  return NULL;
}

struct CondResult *
CreateCondStatement(enum Comparisons bc, struct ExprResult * left, struct ExprResult * right)
{
  CondResult_t * new_cond = malloc(sizeof(CondResult_t));
  if(new_cond)
  {
    new_cond->branchLabel = GenLabel();
    new_cond->branchType = bc;
    if(left != right)
    {
      new_cond->branchCode = AppendSeq(new_cond->branchCode, left->exprCode);
      new_cond->branchCode = AppendSeq(new_cond->branchCode, right->exprCode);
      new_cond->branchCode = AppendSeq(new_cond->branchCode,
        GenInstr(NULL, Branches[bc], TmpRegName(left->resultRegister), TmpRegName(right->resultRegister), new_cond->branchLabel));
        ReleaseTmpReg(left->resultRegister);
        ReleaseTmpReg(right->resultRegister);
        free(left);
        free(right);
    }
    else
    {
      new_cond->branchCode = AppendSeq(new_cond->branchCode, left->exprCode);
      new_cond->branchCode = AppendSeq(new_cond->branchCode,
        GenInstr(NULL, Branches[bc], TmpRegName(left->resultRegister), "$0", new_cond->branchLabel));
        ReleaseTmpReg(left->resultRegister);
        free(left);
    }
    return new_cond;
  }
  return NULL;
}
struct CondResult *
CombineConditionals(enum BoolOperators bc, struct CondResult * left, struct CondResult * right)
{
  CondResult_t * new_cond = malloc(sizeof(CondResult_t));
  InstrSeqList_t * new_branch_code1;
  InstrSeqList_t * new_branch_code2;
  if(new_cond)
  {
    if(left != right)
    {
      if(bc == andOP)
      {
        new_cond->branchLabel = left->branchLabel;
        new_cond->branchType = NeqCmp;
        new_branch_code1 = GenInstr(NULL, CmpSetReg[left->branchType], left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd2);
        //Replace previous left branch instruction with set instruction.
        left->branchCode->tail->Label = new_branch_code1->head->Label;
        left->branchCode->tail->OpCode = new_branch_code1->head->OpCode;
        left->branchCode->tail->Oprnd1 = new_branch_code1->head->Oprnd1;
        left->branchCode->tail->Oprnd2 = new_branch_code1->head->Oprnd2;
        left->branchCode->tail->Oprnd3 = new_branch_code1->head->Oprnd3;
        left->branchCode->tail->next = new_branch_code1->head->next;
        left->branchCode = AppendSeq(left->branchCode, GenInstr(NULL, "beq", left->branchCode->tail->Oprnd1, "$0", new_cond->branchLabel));
        free(new_branch_code1);
        new_cond->branchCode = left->branchCode;

        new_branch_code2 = GenInstr(NULL, CmpSetReg[right->branchType], right->branchCode->tail->Oprnd1, right->branchCode->tail->Oprnd1, right->branchCode->tail->Oprnd2);
        //Replace previous right branch instruction with set instruction.
        right->branchCode->tail->Label = new_branch_code2->head->Label;
        right->branchCode->tail->OpCode = new_branch_code2->head->OpCode;
        right->branchCode->tail->Oprnd1 = new_branch_code2->head->Oprnd1;
        right->branchCode->tail->Oprnd2 = new_branch_code2->head->Oprnd2;
        right->branchCode->tail->Oprnd3 = new_branch_code2->head->Oprnd3;
        right->branchCode->tail->next = new_branch_code2->head->next;
        right->branchCode = AppendSeq(right->branchCode, GenInstr(NULL, "beq", right->branchCode->tail->Oprnd1, "$0", new_cond->branchLabel));
        free(new_branch_code2);

        new_cond->branchCode = AppendSeq(new_cond->branchCode, right->branchCode);
        free(left);
        free(right);
      }
      else if (bc == orOP)
      {
        new_cond->branchLabel = right->branchLabel;
        new_cond->branchType = EqCmp;
        new_branch_code1 = GenInstr(NULL, CmpSetReg[left->branchType], left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd2);
        //Replace previous left branch instruction with set instruction.
        left->branchCode->tail->Label = new_branch_code1->head->Label;
        left->branchCode->tail->OpCode = new_branch_code1->head->OpCode;
        left->branchCode->tail->Oprnd1 = new_branch_code1->head->Oprnd1;
        left->branchCode->tail->Oprnd2 = new_branch_code1->head->Oprnd2;
        left->branchCode->tail->Oprnd3 = new_branch_code1->head->Oprnd3;
        left->branchCode->tail->next = new_branch_code1->head->next;
        left->branchCode = AppendSeq(left->branchCode, GenInstr(NULL, "bne", left->branchCode->tail->Oprnd1, "$0", left->branchLabel));
        free(new_branch_code1);
        new_cond->branchCode = left->branchCode;

        new_branch_code2 = GenInstr(NULL, CmpSetReg[right->branchType], right->branchCode->tail->Oprnd1, right->branchCode->tail->Oprnd1, right->branchCode->tail->Oprnd2);
        //Replace previous right branch instruction with set instruction.
        right->branchCode->tail->Label = new_branch_code2->head->Label;
        right->branchCode->tail->OpCode = new_branch_code2->head->OpCode;
        right->branchCode->tail->Oprnd1 = new_branch_code2->head->Oprnd1;
        right->branchCode->tail->Oprnd2 = new_branch_code2->head->Oprnd2;
        right->branchCode->tail->Oprnd3 = new_branch_code2->head->Oprnd3;
        right->branchCode->tail->next = new_branch_code2->head->next;
        right->branchCode = AppendSeq(right->branchCode, GenInstr(NULL, "bne", right->branchCode->tail->Oprnd1, "$0", left->branchLabel));
        free(new_branch_code2);
        new_cond->branchCode = AppendSeq(new_cond->branchCode, right->branchCode);
        new_cond->branchCode = AppendSeq(new_cond->branchCode, GenInstr(NULL, "b", new_cond->branchLabel, NULL, NULL));
        new_cond->branchCode = AppendSeq(new_cond->branchCode, GenInstr(left->branchLabel, NULL, NULL, NULL, NULL));
        free(left);
        free(right);
      }
    }
    else
    {
      new_cond->branchLabel = left->branchLabel;
      new_cond->branchType = EqCmp;
      new_branch_code1 = GenInstr(NULL, CmpSetReg[left->branchType], left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd2);
      //Replace previous left branch instruction with set instruction.
      left->branchCode->tail->Label = new_branch_code1->head->Label;
      left->branchCode->tail->OpCode = new_branch_code1->head->OpCode;
      left->branchCode->tail->Oprnd1 = new_branch_code1->head->Oprnd1;
      left->branchCode->tail->Oprnd2 = new_branch_code1->head->Oprnd2;
      left->branchCode->tail->Oprnd3 = new_branch_code1->head->Oprnd3;
      left->branchCode->tail->next = new_branch_code1->head->next;
      left->branchCode = AppendSeq(left->branchCode, GenInstr(NULL, "xori", left->branchCode->tail->Oprnd1, left->branchCode->tail->Oprnd1, "1"));
      left->branchCode = AppendSeq(left->branchCode, GenInstr(NULL, "bne", left->branchCode->tail->Oprnd1, "$0", new_cond->branchLabel));
      //free(new_branch_code1);
      new_cond->branchCode = left->branchCode;
      //free(left);
    }
    return new_cond;
  }
  return NULL;
}
struct InstrSeq *
CreateForLoop (struct InstrSeq * assignStmt, struct CondResult * condBlock, struct InstrSeq * codeBlock)
{
  char * aLabel;
  if(condBlock)
  {
    aLabel = GenLabel();
    InstrSeqList_t * fullSequence = AppendSeq(assignStmt,GenInstr(aLabel, NULL, NULL, NULL, NULL));
    fullSequence = AppendSeq(fullSequence, condBlock->branchCode);
    if(codeBlock)
    {
      AppendSeq(fullSequence, codeBlock);
    }
    AppendSeq(fullSequence, GenInstr(NULL, "b", aLabel, NULL, NULL));
    AppendSeq(fullSequence, GenInstr(condBlock->branchLabel, NULL, NULL, NULL, NULL));

    return fullSequence;
  }
  return NULL;
}
struct InstrSeq *
IndefiniteLoop(struct InstrSeq * codeBlock)
{
    char * loop_label = GenLabel();
    char * break_label = GenLabel();
    InstrSeqList_t * infiniteLoop = GenInstr(loop_label, NULL, NULL, NULL, NULL);
    struct InstrSeqNode * node = codeBlock->head;
    while (node) {
      if (node->OpCode) {
  			if(strcmp(node->OpCode, BreakStmt) == 0)
        {
          node->OpCode = strdup("b");
          node->Oprnd1 = break_label;
        }
      }
      node = node->next;
    }
    infiniteLoop = AppendSeq(infiniteLoop, codeBlock);
    infiniteLoop = AppendSeq(infiniteLoop, GenInstr(NULL, "b", loop_label, NULL, NULL));
    infiniteLoop = AppendSeq(infiniteLoop, GenInstr(break_label, NULL, NULL, NULL, NULL));

}
struct InstrSeq *
CreateWhileLoop(struct CondResult * condBlock, struct InstrSeq * codeBlock)
{
  char * aLabel;
  if(condBlock)
  {
    aLabel = GenLabel();
    InstrSeqList_t * fullSequence = GenInstr(aLabel, NULL, NULL, NULL, NULL);
    fullSequence = AppendSeq(fullSequence, condBlock->branchCode);
    if(codeBlock)
    {
      AppendSeq(fullSequence, codeBlock);
    }
    AppendSeq(fullSequence, GenInstr(NULL, "b", aLabel, NULL, NULL));
    AppendSeq(fullSequence, GenInstr(condBlock->branchLabel, NULL, NULL, NULL, NULL));

    return fullSequence;
  }
  return NULL;
}
struct InstrSeq *
StepValue(enum Operators op1, enum Operators op2, char * id)
{
  InstrSeqList_t * step_value;
  SymEntry_t * ident_entry;
  ExprResult_t * temp;
  Attr_t * entry_atr;
  TypeDesc_t * entry_desc;
  int tmpReg;
  if(addOP == op1 && op1 == op2)
  {
    if( (ident_entry = LookupName(IdentifierTable, id)) )
    {
      entry_atr = GetAttr(ident_entry);
      entry_desc = entry_atr->typeDesc;
      if(entry_desc->declType == PrimType)
      {
        if(entry_desc->primDesc == IntBaseType || entry_desc->primDesc == ChrBaseType)
        {
          temp = MakeExprIdent(id);
          temp->exprCode = temp->exprCode = AppendSeq(temp->exprCode, GenInstr(NULL, "addi",TmpRegName(temp->resultRegister), TmpRegName(temp->resultRegister), "1"));
          temp->exprCode = AppendSeq(temp->exprCode, GenInstr(NULL,"sw", TmpRegName(temp->resultRegister),entry_atr->reference,NULL));
          step_value = temp->exprCode;
          ReleaseTmpReg(temp->resultRegister);
          return step_value;
        }
      }
    }
  }
  else if (subOP == op1 && op1 == op2)
  {
    if( (ident_entry = LookupName(IdentifierTable, id)) )
    {
      entry_atr = GetAttr(ident_entry);
      entry_desc = entry_atr->typeDesc;
      if(entry_desc->declType == PrimType)
      {
        if(entry_desc->primDesc == IntBaseType || entry_desc->primDesc == ChrBaseType)
        {
          temp = MakeExprIdent(id);
          temp->exprCode = temp->exprCode = AppendSeq(temp->exprCode, GenInstr(NULL, "addi",TmpRegName(temp->resultRegister), TmpRegName(temp->resultRegister), "-1"));
          temp->exprCode = AppendSeq(temp->exprCode, GenInstr(NULL,"sw", TmpRegName(temp->resultRegister),entry_atr->reference,NULL));
          step_value = temp->exprCode;
          ReleaseTmpReg(temp->resultRegister);
          return step_value;
        }
      }
    }
  }

  return NULL;
}
