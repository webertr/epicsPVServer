#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cadef.h"



void epicsPVExample() {
  

  // Maybe check out CaClient's caExample.c;

  chid fred;

  int status = ca_create_channel("fred", NULL, NULL, 0, &fred);

  SEVCHK(status, "Create channel failed");

  status = ca_pend_io(1.0);

  SEVCHK(status, "Channel connection failed");

  const char *ca_state_to_text[4] = {"Never connected","Not connected", "Connected", "Closed"};

  printf("PV: %s\n", ca_name(fred));
  printf("State: %s\n", ca_state_to_text[ca_state(fred)]);
  printf("Host: %s\n", ca_host_name(fred));
  printf("Read: %s\n", ca_read_access(fred) ? "Y" : "N");
  printf("Write: %s\n", ca_write_access(fred) ? "Y" : "N");
  printf("Type: %s\n", dbr_type_to_text(ca_field_type(fred)));
  //printf("Count: %s\n", ca_element_count(fred));

  struct dbr_time_string val;
  SEVCHK(ca_get(DBR_TIME_STRING, fred, &val), "Get failed");
  printf("value: %s\n", val.value);

  // SEVCHK(ca_clear_channel(fred), "Clear channel failed");
  // Disconnects PV

  // Writting to PV
  // SEVCHK(ca_put(DBR_STRING, fred, "10"), "Put failed");
  // ca_flush_io();

  // If it can hold an array of doubles
  // dbr_double_t data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
  // SEVCHK(ca_array_put(DBR_DOUBLE, 5, fred, data), "Put failed");
  // ca_flush_io();

  // To see other data types available, see See the db_access.h file in Base/include

  // Reading from PV

  //struct dbr_time_double val;
  //const char * severity_to_text[4] = {"No alarm", "Minor", "Major", "Invalid"};
  //SEVCHK(ca_get(DBR_TIME_DOUBLE, fred, &val), "Get failed");
  //SEVCHK(ca_pend_io(1.0), "I/O failed");
  //printf("PV: %s\n", ca_name(fred));
  //printf("value: %g\n", val.value);
  //printf("severity: %s\n", severity_to_text[val.severity]);
  //printf("status: %hd\n", val.status);


}
