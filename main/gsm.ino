#include <GPRS_Shield_Arduino.h>
#include <QueueArray.h>

#define GPRS_PIN_TX    7
#define GPRS_PIN_RX    6
#define BAUDRATE  9600
#define PHONE_NUMBER "xxxxxxxxxxxx"
#define MESSAGE  "hello,world"
GPRS gprs(GPRS_PIN_TX, GPRS_PIN_RX, BAUDRATE);

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

QueueArray <struct gsm_action> gsm_actions;

void gsm_setup()
{
  gprs.checkPowerUp();
  while(!gprs.init()) {
    delay(1000);
    Serial.println("Initialization failed!");
  }
  while(!gprs.isNetworkRegistered()) {
    delay(1000);
    Serial.println("Network has not registered yet!");
  }
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

void gsm_handle_events()
{
  while (!gsm_actions.isEmpty()) {
    struct gsm_action action = gsm_actions.dequeue();
    switch(action.type){
      case SMS:do_send_sms();break;
    }
  }
}

void do_send_sms()
{
  if(gprs.sendSMS(PHONE_NUMBER,MESSAGE)) {
    Serial.print("Send SMS Succeed!\r\n");
  } else {
    Serial.print("Send SMS failed!\r\n");
  }
}

