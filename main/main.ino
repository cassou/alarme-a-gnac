#include <SoftwareSerial.h>
#include <QueueArray.h>
#include "alarm.h"
#include "finished_state_machine.h"
#include "defines.h"

QueueArray <enum fsm_event> events_queue;

void setup() {
  Serial.begin(115200);
  config_load();
  alarm_setup();
  ui_setup();
  rf_setup();
  gsm_setup();
}

void loop() {
  handle_events();
  read_remotes();
  keyboard_polling();
}

void handle_events()
{
  while (!events_queue.isEmpty()) {
    enum fsm_event evt = events_queue.dequeue();
    alarm_handle_events(evt);
    gsm_handle_events(evt);
    ui_handle_events(evt);
  }
}

void push_event(enum fsm_event event)
{
  events_queue.enqueue(event);
}
