#include <Fsm.h>
#include <QueueArray.h>

#include "alarm.h"

#define PIN_LED_RED (10)
#define PIN_LED_GREEN (11)
#define PIN_LED_YELLOW (12)

State state_alarm_disarmed(state_alarm_disarmed_on_enter, NULL, &state_alarm_disarmed_on_exit);
State state_alarm_armed(state_alarm_armed_on_enter, NULL, &state_alarm_armed_on_exit);
State state_alarm_running(state_alarm_running_on_enter, NULL, &state_alarm_running_on_exit);
Fsm fsm_alarm(&state_alarm_disarmed);

QueueArray <enum alarm_event> alarm_events;

void alarm_setup()
{
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, true);
  digitalWrite(PIN_LED_RED, true);
  digitalWrite(PIN_LED_GREEN, true);
  fsm_alarm.add_transition(&state_alarm_disarmed, &state_alarm_armed, EVT_ARM, NULL);
  fsm_alarm.add_transition(&state_alarm_armed, &state_alarm_disarmed, EVT_DISARM, NULL);
  fsm_alarm.add_transition(&state_alarm_armed, &state_alarm_running, EVT_TAMPER, NULL);
  fsm_alarm.add_transition(&state_alarm_running, &state_alarm_disarmed, EVT_DISARM, NULL);
  fsm_alarm.run_machine();//run once otherwise it does not start. Bug in the lib ?
}

void alarm_handle_events()
{
  while (!alarm_events.isEmpty()) {
    fsm_alarm.trigger(alarm_events.dequeue());
  }
}

void alarm_push_event(enum alarm_event event)
{
  alarm_events.enqueue(event);
}

void state_alarm_disarmed_on_enter()
{
  digitalWrite(PIN_LED_GREEN, false);
  myPrintf("%s\n", __FUNCTION__);
}

void state_alarm_disarmed_on_exit()
{
  digitalWrite(PIN_LED_GREEN, true);
  myPrintf("%s\n", __FUNCTION__);
}

void state_alarm_armed_on_enter()
{
  digitalWrite(PIN_LED_RED, false);
  myPrintf("%s\n", __FUNCTION__);
}

void state_alarm_armed_on_exit()
{
  digitalWrite(PIN_LED_RED, true);
  myPrintf("%s\n", __FUNCTION__);
}

void state_alarm_running_on_enter()
{
  digitalWrite(PIN_LED_YELLOW, false);
  myPrintf("%s\n", __FUNCTION__);
  send_sms_to_all();
}

void state_alarm_running_on_exit()
{
  digitalWrite(PIN_LED_YELLOW, true);
  myPrintf("%s\n", __FUNCTION__);
}

