#define KBD_BUFF_LEN 4
static char kbd_buf[KBD_BUFF_LEN+1] = {0}; // last one will always be 0;
static char kbd_buf_index = 0;

void kbd_push(char c)
{
  kbd_buf[kbd_buf_index] = c;
  kbd_buf[kbd_buf_index+1] = 0;
  if(kbd_buf_index+1<KBD_BUFF_LEN) {
    kbd_buf_index++;
  }
  push_event(EVT_KBD);
}

void kbd_erase()
{
  if(kbd_buf_index>0) {
    kbd_buf_index--;
    kbd_buf[kbd_buf_index] = 0;
  }
  push_event(EVT_KBD);
}

void kbd_clear()
{
  memset(kbd_buf, 0, KBD_BUFF_LEN);
  kbd_buf_index = 0;
}

char * keyboard_get_buffer()
{
  return kbd_buf;
}
/* emulate a keyboard via serial for now */
void keyboard_polling()
{
  if (Serial.available()) {
    char c = Serial.read();
    if(c>='0' && c<='9') {
      kbd_push(c);
    }
    if(c=='v'){
      push_event(EVT_KBD);
      push_event(EVT_KBD_VALIDATE);
    }
    if(c=='c'){
      kbd_erase();
      push_event(EVT_KBD);
    }
  }
}