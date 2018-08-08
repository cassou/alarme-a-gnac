#ifndef _ALARM_H_
#define _ALARM_H_

enum alarm_state {
  ALARM_DISARMED,
  ALARM_ARMED,
  ALARM_RUNNING,
};

#define info(x) Serial.print(F(x))

#endif
