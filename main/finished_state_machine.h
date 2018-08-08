#ifndef _FSM_H_
#define _FSM_H_

enum fsm_event {
  EVT_NONE = 0,
  EVT_DISARM,
  EVT_ARM,
  EVT_SOS,
  EVT_HOME,
  EVT_TAMPER = EVT_HOME, // same as EVT_HOME for testing purpose

  /* used for the user interface*/
  EVT_ALARM_STATE_CHANGED,
  EVT_KBD,
  EVT_KBD_VALIDATE,

  /* used for the gsm*/
  EVT_GSM_SUCCESS,
  EVT_GSM_FAILURE,
  EVT_GSM_TIMEOUT,
};

enum event_type {
  EVT_TYPE_CALL,
  EVT_TYPE_GOTO,
  EVT_TYPE_LAST,
};

#define EVT_GOTO(i,s)                                 \
  {                                                   \
    .event_id = i,                                    \
    .type = EVT_TYPE_GOTO,                            \
    {                                                 \
      .goto_step = s                                  \
    }                                                 \
  }

#define EVT_CALL(i,s)                                 \
  {                                                   \
    .event_id = i,                                    \
    .type = EVT_TYPE_CALL,                            \
    {                                                 \
      .on_event = s                                   \
    }                                                 \
  }

#define EVT_LAST()                                    \
  {                                                   \
    .event_id = EVT_NONE,                             \
    .type = EVT_TYPE_LAST                             \
  }

struct fsm_step_t;

struct fsm_event_t{
  enum fsm_event event_id;
  enum event_type type;
  union{
    struct fsm_step_t * (*on_event)();
    struct fsm_step_t * goto_step;
  };
};

struct fsm_step_t{
  void (*on_enter)();
  void (*on_run)();
  struct fsm_event_t * events;
} ;

#endif