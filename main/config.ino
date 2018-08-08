#include <EEPROM.h>
#include "defines.h"

struct config config = {
  CONFIG_VERSION,
  .alarm_state = ALARM_DISARMED,
  .phone_numbers = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
  .remotes = {0,0,0,0,0},
};

void config_load() {
  if (EEPROM.read(0) == CONFIG_VERSION[0] &&
      EEPROM.read(1) == CONFIG_VERSION[1] &&
      EEPROM.read(2) == CONFIG_VERSION[2])
    for (unsigned int t=0; t<sizeof(config); t++){
      *((char*)&config + t) = EEPROM.read(t);
    }
}

void config_save() {
  for (unsigned int t=0; t<sizeof(config); t++){
    EEPROM.write(t, *((char*)&config + t));
  }
}