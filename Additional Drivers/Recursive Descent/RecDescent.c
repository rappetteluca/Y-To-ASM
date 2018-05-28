/* Main of Recursive Descent Parser */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "RDTokens.h"

#include "RDSupport.h"
#include "RDGrammar.h"

int
main(int argc, char * argv[])
{ bool echoMode = true;
  int c;
  opterr = 0;
  while ((c = getopt(argc,argv,"l")) != -1) {
    switch (c) {
      case 'l':
        echoMode = false;
        break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        break;
    }
  }

  if (optind >= argc) {
    fprintf(stderr,"usage: RecDescent [-l] filename\n");
    return 1;
  }

  char * srcName = argv[optind];
  if (!OpenSource(srcName,echoMode)) {
    fprintf(stderr,"failed to open %s\n",srcName);
    return 1;
  }

  // to debug token sequencee, set to true
  RDInit(false);
  if (Prog()) {
    if (CurrentToken() == EOF_TOK) {
      CloseSource();
      printf("Parse Successful\n");
    }
    else {
      CloseSource();
      printf("Parse Successful but excess input\n");
    }
  }
  else {
    CloseSource();
    printf("Parse Unsuccessful\n");
  }

  return 0;
}
