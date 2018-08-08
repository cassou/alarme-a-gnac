#include <LiquidCrystal_I2C.h>
#include "alarm.h"
#include "finished_state_machine.h"

#define PIN_LED_RED (10)
#define PIN_LED_GREEN (11)
#define PIN_LED_YELLOW (12)

const char * TXT_ALARM_ARMED = "ALARM ARMED";
const char * TXT_ALARM_DISARMED = "ALARM DISARMED";
const char * TXT_ALARM_RUNNING = "ALARM RUNNING";

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

const struct fsm_event_t screen_idle_evt[] = {
  EVT_CALL(EVT_KBD, &screen_idle_on_kbd),
  EVT_LAST()
};

const struct fsm_step_t screen_idle = {
  .on_enter = NULL,
  .on_run = screen_idle_run,
  .events = screen_idle_evt,
};

const struct fsm_event_t screen_passwd_evt[] = {
  EVT_CALL(EVT_KBD_VALIDATE, &screen_passwd_on_validate),
  EVT_LAST()
};

const struct fsm_step_t screen_passwd = {
  .on_enter = NULL,
  .on_run = screen_passwd_run,
  .events = screen_passwd_evt,
};

const struct fsm_event_t screen_settings_evt[] = {
  EVT_LAST()
};

const struct fsm_step_t screen_settings = {
  .on_enter = NULL,
  .on_run = screen_settings_run,
  .events = screen_settings_evt,
};

static struct fsm_step_t * ui_current_step = &screen_idle;

void ui_setup()
{
  lcd.begin(20,4);               // initialize the lcd
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, true);
  digitalWrite(PIN_LED_RED, true);
  digitalWrite(PIN_LED_GREEN, true);
  ui_handle_events(EVT_NONE);
}

void ui_handle_events(enum fsm_event evt)
{
  myPrintf("%s %d\n", __FUNCTION__, evt);
  redraw_leds();
  fsm_run(&ui_current_step, evt);
}

void screen_idle_run()
{
  myPrintf("%s\n", __FUNCTION__);
  const char * line2;
  switch (get_alarm_state()) {
    case ALARM_ARMED: line2 = TXT_ALARM_ARMED; break;
    case ALARM_DISARMED: line2 = TXT_ALARM_DISARMED; break;
    case ALARM_RUNNING: line2 = TXT_ALARM_RUNNING; break;
  }
  display_on_screen("SCREEN_IDLE", line2, "Ligne 3", "xxx");
}

struct fsm_step_t * screen_idle_on_kbd()
{
  myPrintf("%s\n", __FUNCTION__);
  return &screen_passwd;
}

void screen_passwd_run()
{
  display_on_screen("SCREEN_PASSWORD", "Enter code", keyboard_get_buffer(), "");
}

struct fsm_step_t * screen_passwd_on_validate()
{
  myPrintf("%s\n", __FUNCTION__);
  struct fsm_step_t * rc = NULL;
  //if this was a real project, make pwd comparision time constant
  if(strcmp(keyboard_get_buffer(), "123") == 0){
    push_event(EVT_DISARM);
    rc = &screen_idle;
  }
  if(strcmp(keyboard_get_buffer(), "456") == 0){
    push_event(EVT_DISARM);
    rc = &screen_settings;
  }
  if(!rc) {
    myPrintf("bad passwd <%s>\n", keyboard_get_buffer);
  }
  kbd_clear();
  return rc;
}

void screen_settings_run()
{
  display_on_screen("SCREEN_SETTINGS", "", "", "");
}

void display_on_screen(const char * line1, const char * line2,
                       const char * line3, const char * line4)
{
  char buf[20+1] = {0};
  memset(buf, ' ', 20);
  memcpy(buf, line1, strnlen(line1, 20));
  lcd.setCursor(0, 0);
  lcd.print(buf);

  memset(buf, ' ', 20);
  memcpy(buf, line2, strnlen(line2, 20));
  lcd.setCursor(0, 1);
  lcd.print(buf);

  memset(buf, ' ', 20);
  memcpy(buf, line3, strnlen(line3, 20));
  lcd.setCursor(0, 2);
  lcd.print(buf);

  memset(buf, ' ', 20);
  memcpy(buf, line4, strnlen(line4, 20));
  lcd.setCursor(0, 3);
  lcd.print(buf);
  // Serial.println("********************");
  // Serial.println(line1);
  // Serial.println(line2);
  // Serial.println(line3);
  // Serial.println(line4);
  // Serial.println("********************");
}

void redraw_leds()
{
  switch (get_alarm_state()){
    case ALARM_DISARMED:
      digitalWrite(PIN_LED_YELLOW, true);
      digitalWrite(PIN_LED_RED, true);
      digitalWrite(PIN_LED_GREEN, false);
      break;
    case ALARM_ARMED:
      digitalWrite(PIN_LED_YELLOW, true);
      digitalWrite(PIN_LED_RED, false);
      digitalWrite(PIN_LED_GREEN, true);
      break;
    case ALARM_RUNNING:
      digitalWrite(PIN_LED_YELLOW, false);
      digitalWrite(PIN_LED_RED, false);
      digitalWrite(PIN_LED_GREEN, true);
      break;
  }
}