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
  EVT_KBD_DOWN,
  EVT_KBD_UP,

  /* used for the gsm*/
  EVT_GSM_SUCCESS,
  EVT_GSM_FAILURE,
  EVT_GSM_TIMEOUT,
  EVT_GSM_BEGIN_SMS,
  EVT_GSM_SMS_DATA,

  EVT_FOUND_REMOTE,
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

#define EVT_CALL(i,s,a)                               \
  {                                                   \
    .event_id = i,                                    \
    .type = EVT_TYPE_CALL,                            \
    {                                                 \
      .call =                                         \
      {                                               \
       .on_event = s,                                 \
       .arg = a                                       \
      }                                               \
    }                                                 \
  }

#define EVT_LAST()                                    \
  {                                                   \
    .event_id = EVT_NONE,                             \
    .type = EVT_TYPE_LAST                             \
  }

#define MENU_EVENTS(menu) \
  EVT_CALL(EVT_KBD_DOWN, &menu_on_down, menu), \
  EVT_CALL(EVT_KBD_UP, &menu_on_up, menu), \
  EVT_CALL(EVT_KBD_VALIDATE, &menu_on_validate, menu)

struct fsm_step_t;

struct fsm_event_t{
  enum fsm_event event_id;
  enum event_type type;
  union {
    struct {
      struct fsm_step_t * (*on_event)(struct fsm_step_t * current_step, void * arg);
      void * arg;
    } call;
    struct fsm_step_t * goto_step;
  };
};

struct fsm_step_t{
  void (*on_enter)();
  void (*on_run)();
  struct fsm_event_t * events;
  unsigned long timeout_ms;
  struct fsm_step_t * (*on_timeout)();
};

struct fsm_t {
  struct fsm_step_t * current_step;
  unsigned long timeout_date;
};

#endif