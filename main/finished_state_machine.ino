#include "alarm.h"
#include "finished_state_machine.h"*

void fsm_run(struct fsm_step_t **current_step, enum fsm_event evt)
{
  if((*current_step)->events) {
    int i = 0;
    for(i=0; (*current_step)->events[i].type!=EVT_TYPE_LAST; i++){
      if((*current_step)->events[i].event_id==evt) {
        break;
      }
    }
    struct fsm_step_t * goto_step = NULL;

    if((*current_step)->events[i].type==EVT_TYPE_CALL && (*current_step)->events[i].on_event) {
      goto_step = (*current_step)->events[i].on_event();
    } else if ((*current_step)->events[i].type==EVT_TYPE_GOTO){
      goto_step = (*current_step)->events[i].goto_step;
    }
    if(goto_step){
      (*current_step) = goto_step;
      if((*current_step)->on_enter) {
        (*current_step)->on_enter();
      }
    }
  }

  if((*current_step)->on_run) {
    (*current_step)->on_run();
  }
}