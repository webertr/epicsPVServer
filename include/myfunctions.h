#ifndef MYFUNCTION_H_   /* Include guard */
#define MYFUNCTION_H_

//#include <epicsStdlib.h>
//#include <epicsString.h>
#include <cadef.h>
#include <tool_lib.h>
//#include <epicsGetopt.h>
//#include <alarm.h>

typedef enum { get, callback } RequestT;

typedef enum { plain, terse, all, specifiedDbr } OutputT;


static int caget (pv *pvs, int nPvs, RequestT request, OutputT format, chtype dbrType, unsigned long reqElems);

void printIPAddress(int argc, char *argv[]);  /* An example function declaration */

void socketTest(void);

void bindSocketTest(void);

void listenTest(void);

void getHTMLFile(char buffer[]);

void epicsPVExample(void);

void caExample(char *pvName);

#endif // MYFUNCTION_H_
