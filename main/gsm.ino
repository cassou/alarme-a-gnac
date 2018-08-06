#include <QueueArray.h>

#define GPRS_PIN_TX    7
#define GPRS_PIN_RX    6
#define PHONE_NUMBER "xxxxxxxxxxxx"
#define GPRS_PIN_POWER 9
#define BAUDRATE       9600
#define GSM_TIMEOUT    5000
#define MESSAGE  "hello,world"

SoftwareSerial mySerial(GPRS_PIN_TX, GPRS_PIN_RX); // RX, TX

enum gsm_action_type {
  SMS,
};

struct gsm_action {
  enum gsm_action_type type;
  union {
    struct {
      uint8_t number_id;
      uint8_t msg_id;
    } sms;
  };
};

struct gsm_step_t;

struct gsm_event_t{
  enum gsm_event event_id;
  struct gsm_step_t * (*on_event)();
};

struct gsm_step_t{
  char name[];
  void (*on_enter)();
  void (*on_run)();
  struct gsm_event_t * events;
} ;

const struct gsm_event_t step_check_power_evt[] = {
  {
    .event_id = GSM_EVT_SUCCESS,
    .on_event = gsmstep_check_power_on_success
  },
  {
    .event_id = GSM_EVT_FAILURE,
    .on_event = gsmstep_check_power_on_failure
  },
  {
    .event_id = GSM_EVT_NONE,
    .on_event = NULL
  }
};

const struct gsm_step_t gsmstep_check_power = {
  "check_power",
  .on_enter = gsmstep_check_power_on_enter,
  .on_run = NULL,
  .events = step_check_power_evt,
};

const struct gsm_event_t step_set_full_fonctionnality_evt[] = {
  {
    .event_id = GSM_EVT_SUCCESS,
    .on_event = gsmstep_set_full_fonctionnality_on_success
  },
  {
    .event_id = GSM_EVT_FAILURE,
    .on_event = gsmstep_set_full_fonctionnality_on_failure
  },
  {
    .event_id = GSM_EVT_NONE,
    .on_event = NULL
  }
};

const struct gsm_step_t gsmstep_set_full_fonctionnality = {
  "set full func",
  .on_enter = gsmstep_set_full_fonctionnality_on_enter,
  .on_run = NULL,
  .events = step_set_full_fonctionnality_evt,
};

const struct gsm_event_t step_check_sim_evt[] = {
  {
    .event_id = GSM_EVT_SUCCESS,
    .on_event = gsmstep_check_sim_on_success
  },
  {
    .event_id = GSM_EVT_FAILURE,
    .on_event = gsmstep_check_sim_on_failure
  },
  {
    .event_id = GSM_EVT_NONE,
    .on_event = NULL
  }
};

const struct gsm_step_t gsmstep_check_sim = {
  "check_sim",
  .on_enter = gsmstep_check_sim_on_enter,
  .on_run = NULL,
  .events = step_check_sim_evt,
};

const struct gsm_event_t step_check_network_evt[] = {
  {
    .event_id = GSM_EVT_SUCCESS,
    .on_event = gsmstep_check_network_on_success
  },
  {
    .event_id = GSM_EVT_FAILURE,
    .on_event = gsmstep_check_network_on_failure
  },
  {
    .event_id = GSM_EVT_NONE,
    .on_event = NULL
  }
};

const struct gsm_step_t gsmstep_check_network = {
  "check_network",
  .on_enter = gsmstep_check_network_on_enter,
  .on_run = NULL,
  .events = step_check_network_evt,
};

const struct gsm_event_t step_idle_evt[] = {
  {
    .event_id = GSM_EVT_NONE,
    .on_event = NULL
  }
};

const struct gsm_step_t gsmstep_idle = {
  "gsm idle",
  .on_enter = NULL,
  .on_run = gsmstep_idle_on_run,
  .events = step_idle_evt,
};

QueueArray <enum gsm_event> gsm_events;
QueueArray <struct gsm_action> gsm_actions;
static struct gsm_step_t * current_step = &gsmstep_check_power;

void gsmstep_check_power_on_enter()
{
  myPrintf("%s\n", __FUNCTION__);
  AT_disable_echo();
}

struct gsm_step_t * gsmstep_check_power_on_failure()
{
  gsm_power_up();
  return &gsmstep_check_power;
}

struct gsm_step_t * gsmstep_check_power_on_success()
{
  info("gsm power is up !\n");
  return &gsmstep_set_full_fonctionnality;
}

void gsmstep_set_full_fonctionnality_on_enter()
{
  myPrintf("%s\n", __FUNCTION__);
  AT_set_full_fonctionnality();
}

struct gsm_step_t * gsmstep_set_full_fonctionnality_on_failure()
{
  return &gsmstep_set_full_fonctionnality;
}

struct gsm_step_t * gsmstep_set_full_fonctionnality_on_success()
{
  info("gsm full func !\n");
  return &gsmstep_check_sim;
}

void gsmstep_check_sim_on_enter()
{
  myPrintf("%s\n", __FUNCTION__);
  AT_check_sim();
}

struct gsm_step_t * gsmstep_check_sim_on_failure()
{
  return &gsmstep_check_sim;
}

struct gsm_step_t * gsmstep_check_sim_on_success()
{
  info("gsm sim ok !\n");
  return &gsmstep_check_network;
}

void gsmstep_check_network_on_enter()
{
  myPrintf("%s\n", __FUNCTION__);
  AT_check_network();
}

struct gsm_step_t * gsmstep_check_network_on_failure()
{
  return &gsmstep_check_network;
}

struct gsm_step_t * gsmstep_check_network_on_success()
{
  info("network ok !");
  return &gsmstep_idle;
}

void gsmstep_idle_on_run()
{
  // myPrintf("%s\n", __FUNCTION__);
  if (!gsm_actions.isEmpty()) {
    struct gsm_action action = gsm_actions.dequeue();
    switch(action.type){
      case SMS:do_send_sms();break;
    }
  }
}

void gsm_setup()
{
  mySerial.begin(9600);
 
/*  while(!gprs.init()) {
    delay(1000);
    Serial.println("Initialization failed!");
  }
  while(!gprs.isNetworkRegistered()) {
    delay(1000);
    Serial.println("Network has not registered yet!");
  }*/
  gsm_push_event(GSM_EVT_NONE); //to start the state machine
}

void send_sms_to_all()
{
  struct gsm_action action;
  action.type = SMS;
  action.sms.number_id = 0;
  action.sms.msg_id = 0;
  gsm_push_action(action);
}

void gsm_push_action(struct gsm_action action)
{
  gsm_actions.enqueue(action);
}

void gsm_push_event(enum gsm_event event)
{
  gsm_events.enqueue(event);
}

void gsm_handle_events()
{
  if(!gsm_events.isEmpty()) {
    enum gsm_event event  = gsm_events.dequeue();
    if(current_step->events) {
      int i = 0;
      for(i=0; current_step->events[i].event_id!=GSM_EVT_NONE; i++){
        if(current_step->events[i].event_id==event) {
          break;
        }
      }
      if(current_step->events[i].on_event) {
        struct gsm_step_t * res = current_step->events[i].on_event();
        if(res){
          current_step = res;
        }
      }
    }
    if(current_step->on_enter) {
      current_step->on_enter();
    }
  }
  if(current_step->on_run) {
    current_step->on_run();
  }
  AT_read();
}

void do_send_sms()
{
/*  if(gprs.sendSMS(PHONE_NUMBER,MESSAGE)) {
    Serial.print("Send SMS Succeed!\r\n");
  } else {
    Serial.print("Send SMS failed!\r\n");
  }*/
}

void gsm_power_up()
{
  // power on pulse for SIM900 Shield
  digitalWrite(GPRS_PIN_POWER,LOW);
  delay(1000);
  digitalWrite(GPRS_PIN_POWER,HIGH);
  delay(2000);
  digitalWrite(GPRS_PIN_POWER,LOW);
  delay(3000);
}
