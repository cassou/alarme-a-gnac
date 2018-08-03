/*
 * IDLE_OFF
 * IDLE_ON
 * ARMING
 * DISARMING
 * MENU
 * ...
 */

State state_screen_idle_off(state_screen_idle_off_on_enter, NULL, NULL);
State state_screen_idle_on(state_screen_idle_on_on_enter, NULL, NULL);
Fsm fsm_screen(&state_screen_idle_on);

void state_screen_idle_on_on_enter()
{
  myPrintf("%s\n", __FUNCTION__);
  display_on_screen("TEST TEST", "POUET");
}

void state_screen_idle_off_on_enter()
{
  myPrintf("%s\n", __FUNCTION__);
}

void ui_init()
{
  fsm_screen.run_machine();//run once otherwise it does not start. Bug in the lib ?
}

void display_on_screen(const char * line1, const char * line2)
{
        Serial.println("********************");
        Serial.println(line1);
        Serial.println(line2);
        Serial.println("********************");
}

