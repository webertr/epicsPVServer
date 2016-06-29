/*caExample.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cadef.h"

void caExample(char *pvName)
{
  double data;
  chid mychid;

  SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");
  SEVCHK(ca_create_channel(pvName,NULL,NULL,10,&mychid),"ca_create_channel failure");
  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
  SEVCHK(ca_get(DBR_DOUBLE,mychid,(void *)&data),"ca_get failure");
  SEVCHK(ca_pend_io(5.0),"ca_pend_io failure");
  printf("%s %f\n",pvName,data);
  return;
}
