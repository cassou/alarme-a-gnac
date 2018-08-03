#ifndef _ALARM_H_
#define _ALARM_H_
enum alarm_event {
  EVT_DISARM,
  EVT_ARM,
  EVT_SOS,
  EVT_HOME,
  EVT_TAMPER = EVT_HOME, // same as EVT_HOME for testing purpose
};
#endif

