#include <SoftwareSerial.h>
#include <QueueArray.h>

#include "alarm.h"

QueueArray <enum alarm_event> alarm_events;

void setup() {
  Serial.begin(115200);
  alarm_setup();
  ui_setup();
  rf_setup();
  gsm_setup();

}

void loop() {
  handle_events();
  gsm_handle_events();
  read_remotes();
  keyboard_polling();
}

void handle_events()
{
  while (!alarm_events.isEmpty()) {
    enum alarm_event evt = alarm_events.dequeue();
    alarm_handle_events(evt);
    ui_handle_events(evt);
  }
}

void push_event(enum alarm_event event)
{
  alarm_events.enqueue(event);
}