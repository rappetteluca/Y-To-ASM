/* Author:      Lucas Rappette
   Created:     02/16.18
   Resources:   I googled an ASCII chart.

                Stack overflow questions that primarily didn't get me anywhere,
                besides one (below).
                https://stackoverflow.com/questions/35968963/trying-to-calculate-logarithm-base-10-without-math-h-really-close-just-having

                https://www.tutorialspoint.com/c_standard_library/string_h.htm

                http://www.pixelbeat.org/programming/gcc/string_buffers.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "IOMngr.h"

//Fancy macro to put things in the middle of two values.
#define CONSTRAIN(x,min,max) ( ((x) < (min)) ? (min) : ( ((x) > (max)) ? (max) : (x)))
#define A_CHAR 65
#define Z_CHAR 90

//Macro For Counting Digits. NOT MINE. I chose base conversion to log10 from ln
//More precision hurts computation. All Floating Point Values suffer from this.
#define LN10 2.302585092994

FILE *sourceFile;
char buffer[MAXLINE];
int bufLen, nextPos;
int curLine;
bool echoMode;
bool needsDisplay; //Not used?
bool EOF_REACHED;

struct message
{
  int startColumn;
  int endColumn;
  char * message;
  bool initialized;
};
#define MAX_MESSAGES 26
struct message messages[MAX_MESSAGES];
int messageCnt;

bool OpenSource(const char * aFileName, bool lineEchoMode)
{
    sourceFile = fopen(aFileName, "r");
    echoMode = lineEchoMode;
    curLine = 0;
    bufLen = 0;
    nextPos = 0;
    EOF_REACHED = false;
    resetMessages();
    if (sourceFile == NULL)
    {
        printf("Failed to open file.\n");
        return false; //could not open file
    }
    return true;
}

void CloseSource()
{
    clearBuffer();
    resetMessages();
    fclose(sourceFile);
}

char GetSourceChar()
{
    char c;
    if(EOF_REACHED)
    {
      return EOF;
    }
    //All Characters read from line
    if(bufLen == nextPos)
    {
        if(bufLen > 0)
        {
            clearBuffer();
            resetMessages();
        }
        curLine += 1;
        bufLen = 0;
        nextPos = 0;
        while((c = fgetc(sourceFile)) != EOF)
        {
            buffer[nextPos] = c;
            ++bufLen;
            ++nextPos;
            if(c == '\n')
            {
              break;
            }
        }
        if(c == EOF)
        {
            buffer[nextPos] = '\0';
        }
        nextPos = 0;
    }

    c = buffer[nextPos];
    if(c == '\0')
    {
      EOF_REACHED = true;
      ++nextPos;
      return EOF;
    }
    ++nextPos;
    return c;
}
/* aColumn is place to put marker. aLength is length of highling, aMessage u no*/
void PostMessage(int aColumn, int aLength, const char * aMessage)
{
    if(messageCnt > 26 || aColumn > bufLen)
    {
        return;
    }
    messages[messageCnt].startColumn = aColumn;
    messages[messageCnt].endColumn = aColumn + aLength;
    if(aMessage)
    {
        messages[messageCnt].message = malloc(sizeof(char) * strlen(aMessage) + 1);
        strcpy(messages[messageCnt].message, aMessage);
    }
    else
    {
        messages[messageCnt].message = NULL;
    }
    messages[messageCnt].initialized = true;
    ++messageCnt;

}
int GetCurrentLine()
{
    return curLine;
}

int GetCurrentColumn()
{
    return nextPos - 1;
}

void resetMessages()
{
    int i;
    for(i = 0; i < MAX_MESSAGES; ++i)
    {
        if(messages[i].initialized)
        {
          messages[i].startColumn = 0;
          messages[i].endColumn = 0;
          messages[i].message = NULL;
          messages[i].initialized = false;
        }
    }
    messageCnt = 0;
}
void displayMessages()
{
    int i;
    for(i = 0; i < messageCnt; ++i)
    {
        if(messages[i].initialized)
        {
          printf("       -%c %s\n", CONSTRAIN(A_CHAR + i, A_CHAR, Z_CHAR),
                                  messages[i].message);
        }
    }
}

void displayLine()
{
    int i = 0;
    int k = 0;
    bool modified = false;

    /* Calculate padding for line formatting */
    int num_digits = 1 + log10( (double) curLine);
    int padding = 5 - num_digits;
    while(padding > 0)
    {
        printf("%c", 32);
        --padding;
    }
    printf("%d: ", curLine);

    /* Prints characters one by one, adding markers as we go */
    while( buffer[k] != '\0')
    {
        /* Minimize Computational Overhead by checking only initialized messages*/
        for(i = 0; i < messageCnt; ++i)
        {
            if(messages[i].initialized && k == messages[i].startColumn)
            {
                /* Do not modify buffer, just display markings */
                printf("\033[7m %c \033[0m", CONSTRAIN(A_CHAR + i, A_CHAR, Z_CHAR));
                printf("\033[4m");
                while(k < messages[i].endColumn)
                {
                    printf("%c", buffer[k]);
                    ++k;
                }
                modified = true;
                break;
            }
        }
        //Simplistic way to reset decorations
        if(modified)
        {
            printf("\033[0m");
            modified = false;
            continue; //Essential instruction.
        }
        printf("%c", buffer[k]);
        ++k;
    }
}

void clearBuffer()
{
    int i;
    //Check to see if buffer should be printed on clear.
    if(!echoMode || containsMessages())
    {
        if(!buffer[0] == '\0')
        {
            displayLine();
        }
        displayMessages();
    }
    for(i = 0; i < bufLen; i++)
    {
        buffer[i] = '\0';
    }
}
/*
* Not my utility function, but you can't make a better natural log algorithm.
*/
double ln(double x)
{
    double old_sum = 0.0;
    double xmlxpl = (x - 1) / (x + 1);
    double xmlxpl_2 = xmlxpl * xmlxpl;
    double denom = 1.0;
    double frac = xmlxpl;
    double term = frac;
    double sum = term;

    while ( sum != old_sum )
    {
        old_sum = sum;
        denom += 2.0;
        frac *= xmlxpl_2;
        sum += frac / denom;
    }
    return 2.0 * sum;
}
/*
*   Cheap trick to find the number of digits in a number. Surpisingly, log10 is
*   more accurate in calculation with a relatively less precise value of LN10.
*/
double log10(double x)
{
    return ln(x) / LN10;
}

bool containsMessages()
{
    return (messageCnt > 0);
}
