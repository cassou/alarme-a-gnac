#ifndef _ALARM_H_
#define _ALARM_H_
enum alarm_event {
  EVT_NONE = 0,
  EVT_DISARM,
  EVT_ARM,
  EVT_SOS,
  EVT_HOME,
  EVT_TAMPER = EVT_HOME, // same as EVT_HOME for testing purpose
  EVT_ALARM_STATE_CHANGED,
  EVT_KBD,
  EVT_KBD_VALIDATE,
};

enum alarm_state {
	ALARM_DISARMED,
	ALARM_ARMED,
	ALARM_RUNNING,
};

#endif

