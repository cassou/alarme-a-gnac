#include <QueueArray.h>
#include "alarm.h"
#include "finished_state_machine.h"

#define GPRS_PIN_TX    7
#define GPRS_PIN_RX    6
#define PHONE_NUMBER   "+33678887642"
#define GPRS_PIN_POWER 9
#define BAUDRATE       115200
#define PHONE_NUMBER   "+33678887642"
#define MESSAGE  "hello,world"

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

extern const struct fsm_step_t PROGMEM gsmstep_set_full_fonctionnality;
extern const struct fsm_step_t PROGMEM gsmstep_check_sim;
extern const struct fsm_step_t PROGMEM gsmstep_set_message_text_mode;
extern const struct fsm_step_t PROGMEM gsmstep_check_network;
extern const struct fsm_step_t PROGMEM gsmstep_idle;
extern const struct fsm_step_t PROGMEM gsmstep_sms_begin;
extern const struct fsm_step_t PROGMEM gsmstep_sms_data;

const struct fsm_event_t PROGMEM step_check_power_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_set_full_fonctionnality),
  EVT_CALL(EVT_GSM_FAILURE, &gsmstep_check_power_on_failure, NULL),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_check_power = {
  .on_enter = AT_disable_echo,
  .on_run = NULL,
  .events = step_check_power_evt,
};

const struct fsm_event_t PROGMEM step_set_full_fonctionnality_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_check_sim),
  EVT_GOTO(EVT_GSM_FAILURE, &gsmstep_set_full_fonctionnality),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_set_full_fonctionnality = {
  .on_enter = AT_set_full_fonctionnality,
  .on_run = NULL,
  .events = step_set_full_fonctionnality_evt,
};

const struct fsm_event_t PROGMEM step_check_sim_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_check_network),
  EVT_GOTO(EVT_GSM_FAILURE, &gsmstep_check_sim),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_check_sim = {
  .on_enter = AT_check_sim,
  .on_run = NULL,
  .events = step_check_sim_evt,
};

const struct fsm_event_t PROGMEM step_check_network_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_set_message_text_mode),
  EVT_GOTO(EVT_GSM_FAILURE, &gsmstep_check_network),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_check_network = {
  .on_enter = AT_check_network,
  .on_run = NULL,
  .events = step_check_network_evt,
};

const struct fsm_event_t PROGMEM step_set_message_text_mode_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_idle),
  EVT_GOTO(EVT_GSM_FAILURE, &gsmstep_set_message_text_mode),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_set_message_text_mode = {
  .on_enter = AT_set_message_text_mode,
  .on_run = NULL,
  .events = step_set_message_text_mode_evt,
};

const struct fsm_event_t PROGMEM step_idle_evt[] = {
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_idle = {
  .on_enter = NULL,
  .on_run = gsmstep_idle_on_run,
  .events = step_idle_evt,
};

const struct fsm_event_t PROGMEM step_sms_begin_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_sms_data),
  EVT_GOTO(EVT_GSM_FAILURE, &gsmstep_idle),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_sms_begin = {
  .on_enter = gsmstep_sms_begin_on_enter,
  .on_run = NULL,
  .events = step_sms_begin_evt,
};

const struct fsm_event_t PROGMEM step_sms_data_evt[] = {
  EVT_GOTO(EVT_GSM_SUCCESS, &gsmstep_idle),
  EVT_GOTO(EVT_GSM_FAILURE, &gsmstep_idle),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM gsmstep_sms_data = {
  .on_enter = gsmstep_sms_data_on_enter,
  .on_run = NULL,
  .events = step_sms_data_evt,
};

QueueArray <struct gsm_action> gsm_actions;
SoftwareSerial mySerial(GPRS_PIN_TX, GPRS_PIN_RX); // RX, TX

static struct fsm_t gsm_fsm = {
  .current_step = &gsmstep_check_power
};


struct fsm_step_t * gsmstep_check_power_on_failure(struct fsm_step_t * current_step, void * arg)
{
  gsm_power_up();
  return &gsmstep_check_power;
}

void gsmstep_idle_on_run()
{
  // myPrintf("%s\n", __FUNCTION__);
  if (!gsm_actions.isEmpty()) {
    struct gsm_action action = gsm_actions.dequeue();
    switch(action.type){
      case SMS:
        push_event(EVT_GSM_BEGIN_SMS);
        break;
    }
  }
}

void gsmstep_sms_begin_on_enter()
{
  AT_begin_sms(PHONE_NUMBER);
}

void gsmstep_sms_data_on_enter()
{
  AT_sms_data("Hello you ;)");
}

void gsm_setup()
{
  mySerial.begin(9600);
  gsm_handle_events(EVT_NONE); //to start the state machine
}

void send_sms_to_all()
{
  struct phone_config phone_config;
  config_load_phone(&phone_config);
  for(int i=0; i<PHONE_NUMBERS_COUNT; i++) {
    if(phone_config.numbers[i][0]!=0){
      struct gsm_action action;
      action.type = SMS;
      action.sms.number_id = i;
      action.sms.msg_id = 0;
      gsm_push_action(action);
    }
  }
}

void gsm_push_action(struct gsm_action action)
{
  gsm_actions.enqueue(action);
}

void gsm_handle_events(enum fsm_event evt)
{
  fsm_run(&gsm_fsm, evt);
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
