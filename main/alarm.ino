#include "alarm.h"
#include "defines.h"
#include "finished_state_machine.h"

void alarm_setup()
{
}

enum alarm_state get_alarm_state()
{
  return config.alarm_state;
}

void alarm_handle_events(enum fsm_event evt)
{
  static enum alarm_state old_state = config.alarm_state;
  switch (config.alarm_state){
    case ALARM_DISARMED:
      if(evt == EVT_ARM) {
        config.alarm_state = ALARM_ARMED;
      }
      break;
    case ALARM_ARMED:
      if(evt == EVT_DISARM) {
        config.alarm_state = ALARM_DISARMED;
      }
      if(evt == EVT_TAMPER) {
        config.alarm_state = ALARM_RUNNING;
      }
      break;
    case ALARM_RUNNING:
      if(evt == EVT_DISARM) {
        config.alarm_state = ALARM_DISARMED;
      }
      break;
  }
  if(old_state != config.alarm_state) {
    old_state = config.alarm_state;
    config_save_state();
    push_event(EVT_ALARM_STATE_CHANGED);
    if(config.alarm_state == ALARM_RUNNING){
      send_sms_to_all();
    }
  }
}
