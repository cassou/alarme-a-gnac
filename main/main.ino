#include <SoftwareSerial.h>
#include <Fsm.h>

#include "alarm.h"

void setup() {
  Serial.begin(115200);
  alarm_setup();
  ui_init();
  rf_setup();
  gsm_setup();

}

void loop() {
  alarm_handle_events();
  gsm_handle_events();
  read_remotes();
}

