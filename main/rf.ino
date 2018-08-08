#include <RCSwitch.h>

#include "alarm.h"
#include "defines.h"

RCSwitch radioReceiver = RCSwitch();
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

void read_remotes() {
  if (radioReceiver.available()) {
    if(radioReceiver.getReceivedBitlength() == REMOTE_BIT_LEN
      && radioReceiver.getReceivedProtocol() == REMOTE_PROTOCOL) {
      uint32_t received_value = radioReceiver.getReceivedValue();
      uint32_t received_id = received_value >> REMOTE_CMD_LEN;
      uint32_t received_cmd = received_value & REMOTE_CMD_MASK;
      for(int i =0; i<REMOTES_COUNT; i++){
        if(config.remotes[i] != 0 && //check the key is not empty
          received_id == config.remotes[i]){
          myPrintf("Received cmd %lu from %lu\n",received_cmd, received_id); 
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

