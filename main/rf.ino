#include <RCSwitch.h>

#include "alarm.h"

RCSwitch radioReceiver = RCSwitch();
enum REMOTE_BUTTONS {
  REMOTE_BUTTON_ARM    =(1<<0),
  REMOTE_BUTTON_DISARM =(1<<1),
  REMOTE_BUTTON_HOME   =(1<<2),
  REMOTE_BUTTON_SOS    =(1<<3),
};

#define REMOTE_PROTOCOL      (1)
#define REMOTE_ID_LEN        (20)
#define REMOTE_CMD_LEN       (4)
#define REMOTE_CMD_MASK      (0b1111)
#define REMOTE_BIT_LEN       (REMOTE_ID_LEN+REMOTE_CMD_LEN)

uint32_t known_remote_count = 1;
uint32_t known_remote[] = {0b11100111101011101101};

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
      for(int i =0; i<known_remote_count; i++){
        if(received_id == known_remote[i]){
          myPrintf("Received cmd %lu from %lu\n",received_cmd, received_id); 
          switch (received_cmd) {
            case REMOTE_BUTTON_ARM:alarm_push_event(EVT_ARM);break;
            case REMOTE_BUTTON_DISARM:alarm_push_event(EVT_DISARM);break;
            case REMOTE_BUTTON_SOS:alarm_push_event(EVT_SOS);break;
            case REMOTE_BUTTON_HOME:alarm_push_event(EVT_HOME);break;
          }
        }
      }
    }
    radioReceiver.resetAvailable();
  }
}

