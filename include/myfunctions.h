#ifndef MYFUNCTION_H_   /* Include guard */
#define MYFUNCTION_H_

#include <cadef.h>
#include <tool_lib.h>


void swapCharArray(char* charArray, char* pattern, char *swapChar);

void cagetFuZE(char *pvName, char *pvValue);

void printIPAddress(int argc, char *argv[]);  /* An example function declaration */

void socketTest(void);

void bindSocketTest(void);

void listenTest(void);

void getHTMLFile(char buffer[]);

void epicsPVExample(void);

void caExample(char *pvName);

#endif // MYFUNCTION_H_
