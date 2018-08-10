#include <LiquidCrystal_I2C.h>
#include "alarm.h"
#include "finished_state_machine.h"

#define PIN_LED_RED (10)
#define PIN_LED_GREEN (11)
#define PIN_LED_YELLOW (12)

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
#define LCD_FLAG_ARROW_UP     (1<<0)
#define LCD_FLAG_ARROW_DOWN   (1<<1)
#define LCD_FLAG_ARROW_RIGHT  (1<<2)
#define LCD_FLAG_OFFSET_RIGHT (1<<3)

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

  byte up_arrow[8] = {
    B00100,
    B01110,
    B10101,
    B00100,
    B00100,
    B00100,
    B00000,
    B00000
  };

  byte down_arrow[8] = {
    B00000,
    B00000,
    B00100,
    B00100,
    B00100,
    B10101,
    B01110,
    B00100
  };
  lcd.createChar(1, up_arrow);
  lcd.createChar(2, down_arrow);


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
  display_line_on_screen(0, F("SCREEN_IDLE"), 0);
  switch (get_alarm_state()) {
    case ALARM_ARMED: display_line_on_screen(1, F("ALARM_ARMED"), 0); break;
    case ALARM_DISARMED: display_line_on_screen(1, F("ALARM_DISARMED"), 0); break;
    case ALARM_RUNNING: display_line_on_screen(1, F("ALARM_RUNNING"), 0); break;
  }
  display_line_on_screen(2, F(""), 0);
  display_line_on_screen(3, F(""), 0);
}

void screen_passwd_run()
{
  display_line_on_screen(0, F("SCREEN_PASSWORD"), 0);
  display_line_on_screen(1, F("Enter code:"), 0);
  display_line_on_screen(2, keyboard_get_buffer(), 0);
  display_line_on_screen(3, F(""), 0);
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
  display_line_on_screen(0, F(""), 0);
  display_line_on_screen(1, F("Press any button"), 0);
  display_line_on_screen(2, F("on the remote"), 0);
  display_line_on_screen(3, F(""), 0);
  return NULL;
}

struct fsm_step_t * screen_remote_scan_found_on_enter(struct fsm_step_t * current_step, void * arg)
{
  display_line_on_screen(0, F(""), 0);
  if(rf_save_learned_remote()){
    display_line_on_screen(1, F("Remote saved"), 0);
    display_line_on_screen(2, F("succesfully"), 0);
  } else {
    display_line_on_screen(1, F("Failed to save"), 0);
    display_line_on_screen(2, F("new remote"), 0);
  }
  display_line_on_screen(3, F(""), 0);
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

 int menu_count = 0;
  while(menu->items[menu_count].text!=NULL) {
    menu_count++;
  }

  display_line_on_screen_P(0, menu->title, 0);
  if(menu->index>0){
      display_line_on_screen(1, "\1", 0);
  } else {
      display_line_on_screen(1, "", 0);
  }

  display_line_on_screen_P(2, menu->items[menu->index].text,0);

  if(menu->index<(menu_count-1)){
      display_line_on_screen(3, "\2", 0);
  } else {
      display_line_on_screen(3, "", 0);
  }
  // int cur_line = 1;
  // int cur_index = 0+menu->index;

  // int right_arrow_line;
  // int start_displaying_index;
  // int display_up_arrow;
  // if(menu->index<3){
  //   right_arrow_line = menu->index+1;
  //   start_displaying_index = 0;
  //   display_up_arrow = 0;
  // } else {
  //   right_arrow_line = 3;
  //   start_displaying_index = menu->index-2;
  //   display_up_arrow = 1;
  // }

  // int menu_count = 0;
  // while(menu->items[menu_count].text!=NULL) {
  //   menu_count++;
  // }

  // for(int i=0; i<3; i++){
  //   int cur_line = i+1;
  //   int flags = LCD_FLAG_OFFSET_RIGHT;
  //   if(cur_line==1 && display_up_arrow) flags|= LCD_FLAG_ARROW_UP;
  //   if(cur_line==3) flags|= LCD_FLAG_ARROW_DOWN;
  //   if(cur_line==right_arrow_line) flags|= LCD_FLAG_ARROW_RIGHT;
  //   if(i+start_displaying_index<menu_count){
  //     display_line_on_screen_P(cur_line, menu->items[i+start_displaying_index].text,flags);
  //   } else {
  //     display_line_on_screen(cur_line, "",flags);
  //   }
  // }
}

void display_line_on_screen(int line, const char * txt, int flags)
{
  char data[20];
  memset(data, ' ', 20);
  for(int i=0; i<20; i++) {
    char c = txt[i];
    if(c==0) break;
    data[i] = c;
  }
  do_display_line_on_screen(line, data, flags);
}

void display_line_on_screen_P(int line, const char * txt, int flags)
{
  display_line_on_screen(line, (const __FlashStringHelper*)txt, flags);
}

void display_line_on_screen(int line, const __FlashStringHelper* txt, int flags)
{
  const char *ptr = (const char *) txt;
  char data[20];
  memset(data, ' ', 20);
  for(int i=0; i<20; i++) {
    char c = pgm_read_byte(ptr+i);
    if(c==0) break;
    data[i] = c;
  }
  do_display_line_on_screen(line, data, flags);
}

void do_display_line_on_screen(int line, char data[20], int flags)
{
  lcd.setCursor(0, line);
  int j = 0;
  if((flags&LCD_FLAG_OFFSET_RIGHT)){
    j = 1;
  }

  for(int i=0; i<20; i++) {
    if((flags&LCD_FLAG_ARROW_UP) && i==19){
      lcd.write(byte(1));
    } else if((flags&LCD_FLAG_ARROW_DOWN) && i==19){
      lcd.write(byte(2));
    } else if((flags&LCD_FLAG_ARROW_RIGHT) && i==0){
      lcd.write('>');
    } else {
      if(i-j>=0) {
        lcd.write(data[i-j]);
      } else {
        lcd.write(' ');
      }
    }
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