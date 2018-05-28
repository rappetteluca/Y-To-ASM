/* Author:      Lucas Rappette
   Created:     02/20.18
   Resources:   http://epaperpress.com/lexandyacc/download/LexAndYaccTutorial.pdf
                https://www.regular-expressions.info/numericranges.html
                https://www.ibm.com/developerworks/library/l-lexyac/index.html
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ScanTokens.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"

// symbol table entries have two attributes, first line of occurrence and occurrence count
struct Attributes {
  int firstLine;
  int firstColumn;
  int cnt;
};
enum AttrKinds { IGNORE_KIND };

// used with DoFoEntries()
void collectEntries(struct SymEntry * entry, int cnt, void * entryArray) {
  ((struct SymEntry **)entryArray)[cnt] = entry;
}

// used with qsort to sort list of symbol table entries
int entryCmp(const void * A, const void * B) {
  // A is pointer to element of array which contains a pointer to a struct SymEntry
  const char * strA = GetName(*(struct SymEntry **)A);
  const char * strB = GetName(*(struct SymEntry **)B);
  return strcmp(strA,strB);
}

int main(int argc, char **argv) {
  int Token;
  char message[265];
  char actionMessage[32];
  struct SymTab * table = NULL;
  struct SymEntry * entry = NULL;
  struct Attributes * attrs = NULL;

  bool ret = OpenSource("ScannerSource",true);
  if (!ret) {
    printf("Could not open source and listing files\n");
    exit(1);
  }

  int intSum = 0;
  float floatSum = 0;

  while ((Token = yylex()) != 0) {
    sprintf(actionMessage,"");
    switch(Token) {
      case INIT_TOK: {
        // create a symbol table
        if (!table) table = CreateSymTab(20,"main",NULL);
      } break;
      case IDENT_TOK: {
        // place the identifier in the table (if it exists), if new then create and init
        // attributes structure, if already in table then update attributes cnt field, in
        // each case set actionMessage with one of
        // sprintf(actionMessage," -- No SymbolTable");
        // sprintf(actionMessage," -- occurrence %d", entry->attributes->cnt);
        // sprintf(actionMessage," -- new ident");
        if(!table)
        {
          sprintf(actionMessage," -- No SymbolTable");
          break;
        }
        if((entry = LookupName(table, yytext)) != NULL)
        {
          attrs = GetAttr(entry);
          if(attrs)
          {
            attrs->cnt += 1;
            sprintf(actionMessage," -- occurrence %d", attrs->cnt);
          }
        }
        else
        {
          entry = EnterName(table, yytext);
          attrs = (struct Attributes *) malloc(sizeof(struct Attributes));
          attrs->firstLine = GetCurrentLine();
          attrs->firstColumn = GetCurrentColumn() - yyleng;
          attrs->cnt = 1;
          SetAttr(entry, IGNORE_KIND, attrs);
          sprintf(actionMessage," -- new ident");
        }
      } break;
      case INT_TOK:
      {
        // It should be noted that in gcc numeric values like "-000000000", "-000000001",
        // "-0.", and "-00000.0" are all ACCEPTED by the C compiler.
        // The leading zeros are trimmed.
        // I am going to follow this standard. Although it is poor.
        intSum += atoi(yytext);
      } break;
      case FLOAT_TOK: {
        // It should be noted that in gcc numeric values like "-000000000", "-000000001",
        // "-0.", and "-00000.0" are all ACCEPTED by the C compiler.
        // The leading zeros are trimmed.
        // I am going to follow this standard. Although it is poor.
        // code
        floatSum += atof(yytext);
      } break;
      case DUMP_TOK: {
        fprintf(stderr,"---------\n");
        fprintf(stderr,"intSum = %d\n",intSum);
        fprintf(stderr,"floatSum = %f\n",floatSum);
        // get table statistics, alloc an array to hold entry pointers
        struct Stats * stats = Statistics(table);
        struct SymEntry ** entries = malloc(stats->entryCnt * sizeof(struct SymEntry *));

        // enumerate the table collecting entry pointers into the array
        InvokeOnEntries(table,false,collectEntries,0,entries);

        // sort the entries
        qsort(entries,stats->entryCnt,sizeof(struct SymEntry *),entryCmp);

        // list the contents of the table in sorted order
        fprintf(stderr,"\nContents of Symbol Table\n");
        fprintf(stderr,"                    name  line   col   cnt \n");
        for (int i = 0; i < stats->entryCnt; i++) {
          fprintf(stderr,"%3d %20s %5d %5d %5d\n", i,
                 GetName(entries[i]),
                 ((struct Attributes *) GetAttr(entries[i]))->firstLine,
                 ((struct Attributes *) GetAttr(entries[i]))->firstColumn,
                 ((struct Attributes *) GetAttr(entries[i]))->cnt);
        }
        free(stats);
      } break;
    }
    sprintf(message,"Token#=%d, Length=%lu, Text=\"%s\"%*c%s",Token,yyleng,yytext,(int)(15-yyleng),' ',actionMessage);
    // depending on the token kind, current column may be at the next char in the input
    // or the last char of the token, if current column minus yyleng is neg force it to zero
    int col = GetCurrentColumn()-yyleng;
    if (col < 0) col = 0;
    PostMessage(col,yyleng,message);
  }
  CloseSource();
}
