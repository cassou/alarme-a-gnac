#include <EEPROM.h>
#include "defines.h"

void config_setup() {
  if (EEPROM.read(0) != CONFIG_VERSION[0] ||
      EEPROM.read(1) != CONFIG_VERSION[1] ||
      EEPROM.read(2) != CONFIG_VERSION[2] ||
      EEPROM.read(3) != CONFIG_VERSION[3]) {
    info("Bad config, erase eeprom\n");
    for (int i=0; i<EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.write(0, CONFIG_VERSION[0]);
    EEPROM.write(1, CONFIG_VERSION[1]);
    EEPROM.write(2, CONFIG_VERSION[2]);
    EEPROM.write(3, CONFIG_VERSION[3]);
  }
}

void config_save_state(enum alarm_state * config)
{
  config_save_partial(config, STATE_CONFIG_OFFSET, STATE_CONFIG_SIZE);
}

void config_load_state(enum alarm_state * config)
{
  config_load_partial(config, STATE_CONFIG_OFFSET, STATE_CONFIG_SIZE);
}

void config_save_remotes(struct remote_config * config)
{
  config_save_partial(config, REMOTE_CONFIG_OFFSET, REMOTE_CONFIG_SIZE);
}

void config_load_remotes(struct remote_config * config)
{
  config_load_partial(config, REMOTE_CONFIG_OFFSET, REMOTE_CONFIG_SIZE);
}

void config_save_pones(struct phone_config * config)
{
  config_save_partial(config, PHONE_CONFIG_OFFSET, PHONE_CONFIG_SIZE);
}

void config_load_phone(struct phone_config * config)
{
  config_load_partial(config, PHONE_CONFIG_OFFSET, PHONE_CONFIG_SIZE);
}

void config_save_partial(void * ptr, unsigned int offset, unsigned int size)
{
  for (unsigned int t=0; t<size; t++){
    EEPROM.write(offset+t, *((char*)ptr + t));
  }
}

void config_load_partial(void * ptr, unsigned int offset, unsigned int size)
{
  for (unsigned int t=0; t<size; t++){
    *((char *)(ptr+t)) = EEPROM.read(offset+t);
  }
}
