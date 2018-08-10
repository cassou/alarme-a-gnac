#include "alarm.h"
#include "finished_state_machine.h"
#include "PROGMEM_readAnything.h"

void fsm_set_current_step(struct fsm_t * fsm, struct fsm_step_t * goto_step)
{
  if(!goto_step) {
    return;
  }
  fsm->current_step = goto_step;

  struct fsm_step_t current_step;
  PROGMEM_readAnything(fsm->current_step, current_step);
  fsm->timeout_date = current_step.timeout_ms + millis();
  if(current_step.on_enter) {
    current_step.on_enter();
  }
}
extern const struct fsm_step_t PROGMEM screen_idle;


void fsm_run(struct fsm_t * fsm, enum fsm_event evt)
{
  myPrintf("%s\n", __FUNCTION__);
  struct fsm_step_t current_step;
  PROGMEM_readAnything(fsm->current_step, current_step);

  if(current_step.events) {

    int i = 0;
    fsm_event_t loaded_event;
    while(1){
      loaded_event = PROGMEM_getAnything(&current_step.events[i]);
      if(loaded_event.type==EVT_TYPE_LAST) break;
      if(loaded_event.event_id==evt) break;
      i++;
    }

    struct fsm_step_t * goto_step = NULL;

    if(loaded_event.type==EVT_TYPE_CALL && loaded_event.call.on_event) {
      goto_step = loaded_event.call.on_event(fsm->current_step, loaded_event.call.arg);
    } else if (loaded_event.type==EVT_TYPE_GOTO){
      goto_step = loaded_event.goto_step;
    }
    fsm_set_current_step(fsm, goto_step);
    PROGMEM_readAnything(fsm->current_step, current_step);
  }

  if(current_step.timeout_ms && millis()>fsm->timeout_date){
    if(current_step.on_timeout) {
      info("timeout");
      struct fsm_step_t * goto_step = current_step.on_timeout();
      fsm_set_current_step(fsm, goto_step);
      PROGMEM_readAnything(fsm->current_step, current_step);
    }
  }

  if(current_step.on_run) {
    current_step.on_run();
  }
}