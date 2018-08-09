#include "alarm.h"
#include "finished_state_machine.h"*

void fsm_set_current_step(struct fsm_t * fsm, struct fsm_step_t * goto_step)
{
  if(!goto_step) {
    return;
  }
  fsm->current_step = goto_step;
  fsm->timeout_date = goto_step->timeout_ms + millis();
  if(fsm->current_step->on_enter) {
    fsm->current_step->on_enter();
  }
}

void fsm_run(struct fsm_t * fsm, enum fsm_event evt)
{
  struct fsm_step_t *current_step = current_step;
  if(fsm->current_step->events) {
    int i = 0;
    for(i=0; fsm->current_step->events[i].type!=EVT_TYPE_LAST; i++){
      if(fsm->current_step->events[i].event_id==evt) {
        break;
      }
    }
    struct fsm_step_t * goto_step = NULL;

    if(fsm->current_step->events[i].type==EVT_TYPE_CALL && current_step->events[i].call.on_event) {
      goto_step = fsm->current_step->events[i].call.on_event(
          fsm->current_step,
          fsm->current_step->events[i].call.arg
        );
    } else if (fsm->current_step->events[i].type==EVT_TYPE_GOTO){
      goto_step = fsm->current_step->events[i].goto_step;
    }
    fsm_set_current_step(fsm, goto_step);
  }

  if(fsm->current_step->timeout_ms && millis()>fsm->timeout_date){
    if(fsm->current_step->on_timeout) {
      info("timeout");
      struct fsm_step_t * goto_step = fsm->current_step->on_timeout();
      fsm_set_current_step(fsm, goto_step);
    }
  }

  if(fsm->current_step->on_run) {
    fsm->current_step->on_run();
  }
}