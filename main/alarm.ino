#include "alarm.h"
#include "finished_state_machine.h"

static enum alarm_state current_state = ALARM_DISARMED;

void alarm_setup()
{
}

enum alarm_state get_alarm_state()
{
  return current_state;
}

void alarm_handle_events(enum fsm_event evt)
{
  static enum alarm_state old_state = current_state;
  switch (current_state){
    case ALARM_DISARMED:
      if(evt == EVT_ARM) {
        current_state = ALARM_ARMED;
      }
      break;
    case ALARM_ARMED:
      if(evt == EVT_DISARM) {
        current_state = ALARM_DISARMED;
      }
      if(evt == EVT_TAMPER) {
        current_state = ALARM_RUNNING;
      }
      break;
    case ALARM_RUNNING:
      if(evt == EVT_DISARM) {
        current_state = ALARM_DISARMED;
      }
      break;
  }
  if(old_state != current_state) {
    old_state = current_state;
    push_event(EVT_ALARM_STATE_CHANGED);
  }
}
