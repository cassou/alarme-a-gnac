#include "alarm.h"
#include "defines.h"
#include "finished_state_machine.h"

void alarm_setup()
{
}

enum alarm_state get_alarm_state()
{
  enum alarm_state tmp ;
  config_load_state(&tmp);
  return tmp;
}

void alarm_handle_events(enum fsm_event evt)
{
  enum alarm_state old_state = get_alarm_state();
  enum alarm_state new_state = old_state;

  switch (old_state){
    case ALARM_DISARMED:
      if(evt == EVT_ARM)    new_state = ALARM_ARMED;
      break;
    case ALARM_ARMED:
      if(evt == EVT_DISARM) new_state = ALARM_DISARMED;
      if(evt == EVT_TAMPER) new_state = ALARM_RUNNING;
      break;
    case ALARM_RUNNING:
      if(evt == EVT_DISARM) new_state = ALARM_DISARMED;
      break;
  }
  if(old_state != new_state) {
    config_save_state(&new_state);
    push_event(EVT_ALARM_STATE_CHANGED);
    if(new_state == ALARM_RUNNING){
      send_sms_to_all();
    }
  }
}
