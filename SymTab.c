/* Author:      Lucas Rappette
   Created:     02/05.18
   Resources:   Past CS421 Linked List assignment
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "SymTab.h"

/* The symbol table entry structure proper.
*/
struct SymEntry {
  struct SymTab * table;
  char *name;
  int attrKind;
  void *attributes;
  struct SymEntry *next;
} typedef SymEntry_t;

/* The symbol table structure proper. The hash table array Contents
   is dynamically allocated according to size
*/
struct SymTab {
  struct SymTab * parent;
  char *scopeName;
  int size;
  struct SymEntry **contents;
} typedef SymTab_t;

struct SymTab *
CreateSymTab(int size, char * scopeName, struct SymTab * parentTable)
{
  int i = 0;
  struct SymTab * newTable = NULL;
  struct SymEntry **newHashTable = NULL;
  newTable = (struct SymTab *) malloc(sizeof(struct SymTab));
  if(!newTable)
  {
    return NULL;
  }
  newHashTable = (struct SymEntry **) malloc(sizeof(struct SymEntry *) * size);
  if(!newHashTable)
  {
    return NULL;
  }
  if(newTable && newHashTable && size > 0)
  {
    for(i = 0; i < size; ++i)
    {
      newHashTable[i] = NULL;
    }
    newTable->parent = parentTable;
    newTable->size = size;
    newTable->contents = newHashTable;

    if (scopeName)
    {
      newTable->scopeName = (char*) malloc(strlen(scopeName) + 1);
      strcpy(newTable->scopeName, scopeName);
    }
    else
    {
      newTable->scopeName = NULL;
    }

    return newTable;
  }
  else
  {
    free(newHashTable);
    free(newTable);

    return NULL;
  }
}

struct SymTab *
DestroySymTab(struct SymTab *aTable)
{
  struct SymTab * parent = NULL;
  struct SymEntry * ptr = NULL;
  int i = 0;
  if(aTable)
  {
    parent = aTable->parent;
    for(i = 0; i < aTable->size; ++i)
    {
      if(aTable->contents[i])
      {
         while(aTable->contents[i])
         {
           ptr = aTable->contents[i];
           aTable->contents[i] = aTable->contents[i]->next;
           if(ptr->name)
           {
             free(ptr->name);
           }
           free(ptr);
         }
      }
    }

    if(aTable->scopeName)
    {
      free(aTable->scopeName);
    }
    free(aTable->contents);
    free(aTable);
  }

  return parent;
}

int
HashName(int size, const char *name)
{
  //Missing Prototype?
  int hashCode = 0;
  int i;
  int len = strlen(name);

  for(i = 0; i < len; ++i)
  {
    hashCode += (int) name[i];
  }
  //hashCode = 31 * hashCode;
  //hashCode = 29 * len + hashCode;

  return hashCode % size;
}

struct SymEntry *
LookupName(struct SymTab *aTable, const char * name)
{
  int hash;
  struct SymEntry * entryPtr;
  if(aTable == NULL || name == NULL)
  {
    return NULL;
  }
  else
  {
    hash = HashName(aTable->size, name);

    if(aTable->contents[hash])
    {
      entryPtr = aTable->contents[hash];
      while(entryPtr != NULL)
      {
        if(strcmp(entryPtr->name, name) == 0)
        {
          return entryPtr;
        }
        else
        {
          entryPtr = entryPtr->next;
        }
      }
    }

    return LookupName(aTable->parent, name);
  }
}

struct SymEntry *
EnterName(struct SymTab *aTable, const char *name)
{
  struct SymTab * temp;
  struct SymEntry * entry;

  if(aTable && name)
  {
    int hash;
    temp = aTable->parent;
    //Cheap trick to end recursion in LookupName
    aTable->parent = NULL;
    entry = LookupName(aTable, name);
    aTable->parent = temp;

    if(entry)
    {
      return entry;
    }
    else
    {
      hash = HashName(aTable->size, name);
      entry = (struct SymEntry *) malloc(sizeof(struct SymEntry));
      entry->name = strdup(name);
      entry->table = aTable;
      entry->next = aTable->contents[hash];
      entry->attrKind = 0;
      entry->attributes = NULL;
      aTable->contents[hash] = entry;
      return entry;
    }
  }
  else
  {
    printf("Error: Invalid table address or name given as input. (NULL is an invalid name)\n");
    return NULL;
  }
}

void
SetAttr(struct SymEntry *anEntry, int kind, void *attributes)
{
  if (anEntry)
  {
    anEntry->attrKind = kind;
    anEntry->attributes = attributes;
  }
}

int
GetAttrKind(struct SymEntry *anEntry)
{
  if(anEntry)
  {
    return anEntry->attrKind;
  }

  return 0;
}

void *
GetAttr(struct SymEntry *anEntry)
{
  if(anEntry)
  {
    return anEntry->attributes;
  }

  return NULL;
}

const char *
GetName(struct SymEntry *anEntry)
{
  if(anEntry)
  {
    return anEntry->name;
  }

  return NULL;
}

struct SymTab *
GetTable(struct SymEntry *anEntry)
{
  if(anEntry)
  {
    return anEntry->table;
  }

  return NULL;
}

const char *
GetScopeName(struct SymTab *aTable)
{
  if(aTable)
  {
    return aTable->scopeName;
  }

  return NULL;
}

char *
GetScopePath(struct SymTab *aTable)
{
  int len = 0;
  char *path;
  struct SymTab * pointer = aTable;

  while(pointer != NULL)
  {
    len += strlen(GetScopeName(pointer));
    len += 1;
    pointer = pointer->parent;
  }

  path = (char *) malloc(len);
  path = RecurseScopePath(aTable, path);
  //printf("Generated String: %s\n", path);
  return path;
}

char *
RecurseScopePath(struct SymTab *aTable, char * dest)
{
  if(aTable->parent == NULL)
  {
    return strcpy(dest, GetScopeName(aTable));
  }
  else
  {
    strcat(RecurseScopePath(aTable->parent, dest), ">");
    return strcat(dest, GetScopeName(aTable));
  }
}


struct SymTab *
GetParentTable(struct SymTab *aTable)
{
  if(aTable)
  {
    return aTable->parent;
  }

  return NULL;
}

void
InvokeOnEntries(struct SymTab *aTable, bool includeParentTable,
             entryWorkFunc workFunc, int startCnt, void * withArgs)
{
  int i;
  struct SymEntry * ptr = NULL;
  struct SymEntry * tmp = NULL;
  for(i = 0; i < aTable->size; ++i)
  {
    ptr = aTable->contents[i];
    if(ptr)
    {
      while(ptr != NULL)
      {
        tmp = ptr->next;
        workFunc(ptr, startCnt, withArgs);
        ++startCnt;

        ptr = tmp;
      }
    }
  }
  if(includeParentTable && aTable->parent)
  {
    InvokeOnEntries(aTable->parent, true, workFunc, startCnt, withArgs);
  }
  free(ptr);
}

struct Stats *
Statistics(struct SymTab *aTable)
{
  int i;
  int chainLength;
  bool minSet = false;
  struct Stats * stats = NULL;
  struct SymEntry * ptr = NULL;

  stats = (struct Stats *) malloc(sizeof(struct Stats));
  stats->minLen = INT_MAX;
  stats->maxLen = 0;
  stats->avgLen = 0;
  stats->entryCnt = 0;

  for(i = 0; i < aTable->size; ++i)
  {
    ptr = aTable->contents[i];
    chainLength = 0;
    while(ptr != NULL)
    {
      chainLength += 1;
      stats->entryCnt += 1;
      ptr = ptr->next;
    }

    if(chainLength < stats->minLen || minSet == false)
    {
      stats->minLen = chainLength;
      minSet = true;
    }
    if(chainLength > stats->maxLen)
    {
      stats->maxLen = chainLength;
    }
  }
  stats->avgLen = stats->entryCnt / aTable->size;
  return stats;
}
