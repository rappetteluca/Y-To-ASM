/* IOMngr.h

   Implements a simple interface for obtaining source file characters one at
   a time. It also produces a listing of the source file, together
   with any messages, to stdout. Depending on the configured mode, either all
   source lines are listed or only lines with associated messages.

*/
#ifndef IOMNGR_H
#define IOMNGR_H

#include <stdbool.h>

#define MAXLINE 1024

/* OpenSource     Opens aFileName as the source and sets the lineEchoMode.
                  If lineEchoMode is false then all source lines are written,
                  along with associated messages, to stdout as they are
                  completed. If lineEchoMode is true then only source lines
                  that have associated messages are written to stdout.

   CloseSource    Closes the source file and performs other final actions.
*/
bool OpenSource(const char * aFileName, bool lineEchoMode);
void CloseSource();

/* GetSourceChar  Returns the next character in the source file. This may result
                  in the current line and associated messages being written to
                  stdout. The EOF character is returned if the source file is
                  at the end of file when this function is called.
*/
char GetSourceChar();

/* PostMessage    Registers a message to be associated with the current line
                  and indicated by a marker at aColumn with aLength many
                  characters written in underline mode. The letters 'A' ... 'Z'
                  are used as the markers, consequently, a maximum of 26
                  message lines are allowed for each source line. Calls to
                  PostMessage beyond this limit are silently ignored as are calls
                  with aColumn outside the range of the current line or overlaping
                  the prior message.
*/
void PostMessage(int aColumn, int aLength, const char * aMessage);

/* GetCurrentLine Returns the current line number in the source file.

   GetCurrentColumn Returns the column number of the most recently returned
                  character.
*/
int GetCurrentLine();
int GetCurrentColumn();

/* resetMessages    Sets all line messages back to uninitialized.

   containsMessages Returns true if the current line has messages to include.

   clearBuffer      Possibly prints buffered characters to stdout
                    followed by resetting the buffer.

   displayLine      Prints the currently buffered characters to stdout with
                    special markings/messages.

   displayMessages  Prints out line message(s) and their associated line token(s)

   ln(x)            Returns a double representing natural log of x.

   log10(x)         Returns a double representing log base 10 of x.
*/
void resetMessages();
bool containsMessages();
void clearBuffer();
void displayLine();
void displayMessages();
double ln(double x);
double log10(double x);

#endif
