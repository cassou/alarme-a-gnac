#include <RCSwitch.h>

#include "alarm.h"
#include "defines.h"

RCSwitch radioReceiver = RCSwitch();
bool learning_mode = false;
uint32_t learned_id = 0;

enum REMOTE_BUTTONS {
  REMOTE_BUTTON_ARM    =(1<<0),
  REMOTE_BUTTON_DISARM =(1<<1),
  REMOTE_BUTTON_HOME   =(1<<2),
  REMOTE_BUTTON_SOS    =(1<<3),
};

// uint32_t known_remote[] = {0b11100111101011101101};

void rf_setup()
{
  radioReceiver.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void rf_toggle_learning_mode(bool enabled)
{
  learning_mode = enabled;
}

uint32_t rf_get_learned_id()
{
  return learned_id;
}

bool rf_is_known_remote(uint32_t id)
{
  struct remote_config config;
  config_load_remotes(&config);

  for(int i =0; i<REMOTES_COUNT; i++){
    if(config.remotes[i] != 0 && //check the key is not empty
      id == config.remotes[i]){
      return true;
    }
  }
  return false;
}

bool rf_save_learned_remote()
{
  if(rf_is_known_remote(learned_id)){
    return true;
  }

  struct remote_config config;
  config_load_remotes(&config);
  for(int i =0; i<REMOTES_COUNT; i++){
    if (config.remotes[i] == 0) {
      config.remotes[i] = learned_id;
      config_save_remotes(&config);
      return true;
    }
  }
  return false;
}

void read_remotes() {
  if (radioReceiver.available()) {
    if(radioReceiver.getReceivedBitlength() == REMOTE_BIT_LEN
      && radioReceiver.getReceivedProtocol() == REMOTE_PROTOCOL) {
      uint32_t received_value = radioReceiver.getReceivedValue();
      uint32_t received_id = received_value >> REMOTE_CMD_LEN;
      uint32_t received_cmd = received_value & REMOTE_CMD_MASK;
      if(learning_mode){
        push_event(EVT_FOUND_REMOTE);
        learned_id = received_id;
        rf_toggle_learning_mode(false);
      } else {
        if(rf_is_known_remote(received_id)){
          //myPrintf("Received cmd %lu from %lu\n",received_cmd, received_id);
          switch (received_cmd) {
            case REMOTE_BUTTON_ARM:push_event(EVT_ARM);break;
            case REMOTE_BUTTON_DISARM:push_event(EVT_DISARM);break;
            case REMOTE_BUTTON_SOS:push_event(EVT_SOS);break;
            case REMOTE_BUTTON_HOME:push_event(EVT_HOME);break;
          }
        }
      }
    }
    radioReceiver.resetAvailable();
  }
}

