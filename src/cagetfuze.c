/****************************************************************************
 * cagetfuze.c - This is pulled from Ralph Lange's code for the caget EPICS tool.
 * I made some crude modifications (emphasis on the word crude), so that I 
 * could call the function cagetfuze(), and have it return PV values
 ***************************************************************************/


/************************************************************************* \
* Copyright (c) 2009 Helmholtz-Zentrum Berlin fuer Materialien und Energie.
* Copyright (c) 2006 Diamond Light Source Ltd.
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* Copyright (c) 2002 Berliner Elektronenspeicherringgesellschaft fuer
*     Synchrotronstrahlung.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 *  Author: Ralph Lange (BESSY)
 *
 *  Modification History
 *  2006/01/17 Malcolm Walters (Tessella/Diamond Light Source)
 *     Fixed problem with "-c -w 0" hanging forever
 *  2008/04/16 Ralph Lange (BESSY)
 *     Updated usage info
 *  2009/03/31 Larry Hoff (BNL)
 *     Added field separators
 *  2009/04/01 Ralph Lange (HZB/BESSY)
 *     Added support for long strings (array of char) and quoting of nonprintable characters
 *
 */


#include <stdio.h>
#include <string.h>
#include <epicsStdlib.h>
#include <epicsString.h>

#include <alarm.h>
#include <cadef.h>
#include <epicsGetopt.h>

#include <tool_lib.h>

#define VALID_DOUBLE_DIGITS 18  /* Max usable precision for a double */
#define PEND_EVENT_SLICES 5     /* No. of pend_event slices for callback requests */

/* Different output formats */
typedef enum { plain, terse, all, specifiedDbr } OutputT;

/* Different request types */
typedef enum { get, callback } RequestT;

static int nConn = 0;           /* Number of connected PVs */
static int nRead = 0;           /* Number of channels that were read */
static int floatAsString = 0;   /* Flag: fetch floats as string */






/***************************************************************************
 *
 * Function:event_handler
 *
 * Description:CA event_handler for request type callback
 * Allocates the dbr structure and copies the data.
 * This came with the source code. It is static so won't pollute anything else
 *
 * Arg(s) In:args  -  event handler args (see CA manual)
 *
 ***************************************************************************/

static void event_handler (evargs args)
{
  pv* ppv = args.usr;

  ppv->status = args.status;
  if (args.status == ECA_NORMAL)
    {
      ppv->dbrType = args.type;
      ppv->value   = calloc(1, dbr_size_n(args.type, args.count));
      memcpy(ppv->value, args.dbr, dbr_size_n(args.type, args.count));
      ppv->nElems = args.count;
      nRead++;
    }
}



/******************************************************************************
 * Function: cagetFuZE
 * Inputs: char *, char*
 * Returns: int (1 if successful, -1 if error)
 * Description: This is a crude modification to the caget command line tool.
 * I have changed it so you pass it a pvName, and a char pointer for the pv value
 * (or what ascii text would be ouput to the command line. It will then update
 * the char pointer pvValue to what would be output to the console by caget.
 * I don't know anything about character limits, so I'm unsure about the size of pvValue
 * HOW MUCH MEMORY TO ALLOCATE TO PVVALUE?????
 ******************************************************************************/

int cagetFuZE(char *pvName, char *pvValue) {

  RequestT request = get;
  OutputT format = plain;
  chtype dbrType = -1;
  unsigned long reqElems = 0;
  int n, result;
  pv* pvs;
  int i;
  unsigned long nElems;
  

  result = ca_context_create(ca_disable_preemptive_callback);

  pvs = calloc(1, sizeof(pv));

  pvs[0].name = pvName;
  
  connect_pvs(pvs, 1);
  
  for (n = 0; n < 1; n++) {
    
    /* Set up pvs structure */
    /* -------------------- */

    /* Get natural type and array count */
    nElems         = ca_element_count(pvs[n].chid);
    pvs[n].dbfType = ca_field_type(pvs[n].chid);
    pvs[n].dbrType = dbrType;

    /* Set up value structures */
    if (format != specifiedDbr)
      {
	pvs[n].dbrType = dbf_type_to_DBR_TIME(pvs[n].dbfType); /* Use native type */
	if (dbr_type_is_ENUM(pvs[n].dbrType))                  /* Enums honour -n option */
	  {
	    if (enumAsNr) pvs[n].dbrType = DBR_TIME_INT;
	    else          pvs[n].dbrType = DBR_TIME_STRING;
	  }
	else if (floatAsString &&
		 (dbr_type_is_FLOAT(pvs[n].dbrType) || dbr_type_is_DOUBLE(pvs[n].dbrType)))
	  {
	    pvs[n].dbrType = DBR_TIME_STRING;
	  }
      }

    /* Issue CA request */
    /* ---------------- */

    if (ca_state(pvs[n].chid) == cs_conn)
      {
  	nConn++;
  	pvs[n].onceConnected = 1;
  	if (request == callback)
  	  {
  	    /* Event handler will allocate value and set nElems */
  	    pvs[n].reqElems = reqElems > nElems ? nElems : reqElems;
  	    result = ca_array_get_callback(pvs[n].dbrType,
  					   pvs[n].reqElems,
  					   pvs[n].chid,
  					   event_handler,
  					   (void*)&pvs[n]);
  	  } else {
  	  /* We allocate value structure and set nElems */
	  pvs[n].nElems = reqElems && reqElems < nElems ? reqElems : nElems;
  	  pvs[n].value = calloc(1, dbr_size_n(pvs[n].dbrType, pvs[n].nElems));
	  if (!pvs[n].value) {
  	    fprintf(stderr,"Memory allocation failed\n");
  	    return -1;
  	  }
  	  result = ca_array_get(pvs[n].dbrType,
  				pvs[n].nElems,
  				pvs[n].chid,
  				pvs[n].value);
  	}
  	pvs[n].status = result;
      } else {
      pvs[n].status = ECA_DISCONN;
    }
  }

  if (!nConn) return -1;              /* No connection? We're done. */

  /* Wait for completion */
  /* ------------------- */

  result = ca_pend_io(caTimeout);
  if (result == ECA_TIMEOUT) {
    fprintf(stderr, "Read operation timed out: some PV data was not read.\n");
    return -1;
  }

  if (request == callback)    /* Also wait for callbacks */
    {
      if (caTimeout != 0)
        {
	  double slice = caTimeout / PEND_EVENT_SLICES;
	  for (n = 0; n < PEND_EVENT_SLICES; n++)
            {
	      ca_pend_event(slice);
	      if (nRead >= nConn) break;
            }
	  if (nRead < nConn) {
	    fprintf(stderr, "Read operation timed out: some PV data was not read.\n");
	    return -1;
	  }
	} else {
	/* For 0 timeout keep waiting until all are done */
	    while (nRead < nConn) {
	      ca_pend_event(1.0);
	    }
      }
    }


  // Does this kill the connection??
  ca_context_destroy();
  
  sprintf(pvValue, "%s\n", val2str(pvs[0].value, pvs[0].dbrType, 0));

  return 1;

}
