#include <LiquidCrystal_I2C.h>
#include "alarm.h"
#include "finished_state_machine.h"

#define PIN_LED_RED (10)
#define PIN_LED_GREEN (11)
#define PIN_LED_YELLOW (12)

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

extern const struct fsm_step_t PROGMEM screen_passwd;
extern const struct fsm_step_t PROGMEM screen_remote_scan;
extern const struct fsm_step_t PROGMEM screen_remote_scan_found;



PROGMEM const char REMOTE[] = "REMOTES SETTINGS";
PROGMEM const char REMOTE_ADD[] = "Add a remote";
PROGMEM const char REMOTE_DEL[] = "Delete a remote";
PROGMEM const char REMOTE_TOTO1[] = "toto 1";
PROGMEM const char REMOTE_TOTO2[] = "toto 2";
PROGMEM const char REMOTE_TOTO3[] = "toto 3";

struct menu_item {
  const char * text;
  struct fsm_step_t * goto_step;
};

struct menu {
  const char * title;
  const struct menu_item * items;
  uint8_t index;
};

const struct menu_item mi_remotes[] =
{
  {REMOTE_ADD, &screen_remote_scan},
  {REMOTE_DEL, &screen_passwd},
  {REMOTE_TOTO1, &screen_passwd},
  {REMOTE_TOTO2, &screen_passwd},
  {REMOTE_TOTO3, &screen_passwd},
  {NULL, NULL}
};

struct menu menu_remotes = {REMOTE, mi_remotes, 0};


const struct fsm_event_t PROGMEM screen_idle_evt[] = {
  EVT_GOTO(EVT_KBD, &screen_passwd),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM screen_idle = {
  .on_enter = NULL,
  .on_run = screen_idle_run,
  .events = screen_idle_evt,
};

const struct fsm_event_t PROGMEM screen_passwd_evt[] = {
  EVT_CALL(EVT_KBD_VALIDATE, &screen_passwd_on_validate, NULL),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM screen_passwd = {
  .on_enter = NULL,
  .on_run = screen_passwd_run,
  .events = screen_passwd_evt,
};

const struct fsm_event_t PROGMEM screen_settings_evt[] = {
  MENU_EVENTS(&menu_remotes),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM screen_settings = {
  .on_enter = NULL,
  .on_run = screen_settings_run,
  .events = screen_settings_evt,
};

const struct fsm_event_t PROGMEM screen_remote_scan_evt[] = {
  EVT_GOTO(EVT_FOUND_REMOTE, &screen_remote_scan_found),
  EVT_LAST()
};

const struct fsm_step_t PROGMEM screen_remote_scan = {
  .on_enter = screen_remote_scan_on_enter,
  .on_run = NULL,
  .events = screen_remote_scan_evt,
};

const struct fsm_event_t PROGMEM screen_remote_scan_found_evt[] = {
  EVT_LAST()
};

const struct fsm_step_t PROGMEM screen_remote_scan_found = {
  .on_enter = screen_remote_scan_found_on_enter,
  .on_run = NULL,
  .events = screen_remote_scan_found_evt,
  .timeout_ms = 5000,
  .on_timeout = screen_remote_scan_found_on_timeout
};

static struct fsm_t ui_fsm = {
  .current_step = &screen_idle
};

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
  fsm_run(&ui_fsm, evt);
}

void screen_idle_run()
{
  myPrintf("%s %d\n", __FUNCTION__, get_alarm_state());
  display_line_on_screen(0, F("SCREEN_IDLE"));
  switch (get_alarm_state()) {
    case ALARM_ARMED: display_line_on_screen(1, F("ALARM_ARMED")); break;
    case ALARM_DISARMED: display_line_on_screen(1, F("ALARM_DISARMED")); break;
    case ALARM_RUNNING: display_line_on_screen(1, F("ALARM_RUNNING")); break;
  }
  display_line_on_screen(2, F(""));
  display_line_on_screen(3, F(""));
}

void screen_passwd_run()
{
  display_line_on_screen(0, F("SCREEN_PASSWORD"));
  display_line_on_screen(1, F("Enter code:"));
  display_line_on_screen(2, keyboard_get_buffer());
  display_line_on_screen(3, F(""));
}

struct fsm_step_t * screen_passwd_on_validate(struct fsm_step_t * current_step, void * arg)
{
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

struct fsm_step_t *  menu_on_down(struct fsm_step_t * current_step, void * arg)
{
  menu_next((struct menu *)arg);
  return NULL;
}

struct fsm_step_t * menu_on_up(struct fsm_step_t * current_step, void * arg)
{
  menu_prev((struct menu *)arg);
  return NULL;
}

struct fsm_step_t * menu_on_validate(struct fsm_step_t * current_step, void * arg)
{
  struct menu * m = (struct menu *)arg;
  return m->items[m->index].goto_step;
}

void screen_settings_run()
{
  menu_render(&menu_remotes);
}

struct fsm_step_t * screen_remote_scan_on_enter(struct fsm_step_t * current_step, void * arg)
{
  myPrintf("%s\n", __FUNCTION__);
  rf_toggle_learning_mode(true);
  display_line_on_screen(0, F(""));
  display_line_on_screen(1, F("Press any button"));
  display_line_on_screen(2, F("on the remote"));
  display_line_on_screen(3, F(""));
  return NULL;
}

struct fsm_step_t * screen_remote_scan_found_on_enter(struct fsm_step_t * current_step, void * arg)
{
  display_line_on_screen(0, F(""));
  if(rf_save_learned_remote()){
    display_line_on_screen(1, F("Remote saved"));
    display_line_on_screen(2, F("succesfully"));
  } else {
    display_line_on_screen(1, F("Failed to save"));
    display_line_on_screen(2, F("new remote"));
  }
  display_line_on_screen(3, F(""));
  return NULL;
}

struct fsm_step_t * screen_remote_scan_found_on_timeout()
{
  return &screen_settings;
}

void menu_next(struct menu * menu)
{
  if(menu->items[menu->index+1].text!=NULL) {
    menu->index++;
  }
}

void menu_prev(struct menu * menu)
{
  if(menu->index>0) {
    menu->index--;
  }
}

void menu_render(struct menu * menu)
{
  display_line_on_screen_P(0, menu->title);
  int cur_line = 1;
  int cur_index = 0+menu->index;
  while (cur_line<4){
    if(menu->items[cur_index].text!=NULL) {
      display_line_on_screen_P(cur_line, menu->items[cur_index].text);
      cur_index++;
    } else {
      display_line_on_screen(cur_line, "");
    }
    cur_line++;
  }
}

void display_line_on_screen(int line, const char * txt)
{
  lcd.setCursor(0, line);
  lcd.print(txt);
  int cnt = strlen(txt);

  for(cnt; cnt<20; cnt++){
    lcd.print(' ');
  }
}

void display_line_on_screen_P(int line, const char* cmd)
{
  display_line_on_screen(line, (const __FlashStringHelper*)cmd);
}

void display_line_on_screen(int line, const __FlashStringHelper* cmd)
{
  lcd.setCursor(0, line);
  const char *ptr = (const char *) cmd;
  byte b;
  do {
    b = pgm_read_byte(ptr++);
    if (b) {
      lcd.print((char)b);
    }
  } while (b);

  int cnt = (unsigned int)ptr-(unsigned int)cmd;
  for(cnt; cnt<=20; cnt++){
    lcd.print(' ');
  }
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