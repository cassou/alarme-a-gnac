#define GSM_TIMEOUT    5000

static const char at_resp_ok[]          = "OK\r\n";
static const char at_resp_pin_ready[]   = "+CPIN: READY\r\n";
static const char at_resp_net_home[]    = "+CREG: 0,1\r\n";
static const char at_resp_net_roaming[] = "+CREG: 0,5\r\n";
static const char at_resp_prompt[]           = ">\r\n";

static char receive_buff[128];
static uint8_t receive_buff_index = 0;
#define MAX_EXPECTED_RESPONSES (2)

static struct  {
  char * expected_responses[MAX_EXPECTED_RESPONSES];
  unsigned long timeout_date = 0; //same type as millis
  void (*cb)(bool success);
} current_at_cmd;

void AT_disable_echo()
{
  AT_send(F("ATE0\r\n"));
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_ok;
  current_at_cmd.expected_responses[1] = NULL;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_set_full_fonctionnality()
{
  info(">>");
  AT_send(F("AT+CFUN=1\r\n"));
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_ok;
  current_at_cmd.expected_responses[1] = NULL;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_check_sim()
{
  info(">>");
  AT_send(F("AT+CPIN?\r\n"));
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_pin_ready;
  current_at_cmd.expected_responses[1] = NULL;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_check_network()
{
  info(">>");
  AT_send(F("AT+CREG?\r\n"));
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_net_home;
  current_at_cmd.expected_responses[1] = at_resp_net_roaming;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_set_message_text_mode()
{
  info(">>");
  AT_send(F("AT+CMGF=1\r\n"));
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_ok;
  current_at_cmd.expected_responses[1] = NULL;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_begin_sms(const char * number)
{
  info(">>");
  AT_send(F("AT+CMGS=\"+33678887642\"\r"));
  delay(100);
  // AT_send("trololo");
  // AT_send((char)26);
  // AT_send(number);
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_prompt;
  current_at_cmd.expected_responses[1] = NULL;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_sms_data(const char * data)
{
  info(">>");
  AT_send(data);
  AT_send((char)26);
  current_at_cmd.timeout_date = millis() + GSM_TIMEOUT;
  current_at_cmd.expected_responses[0] = at_resp_ok;
  current_at_cmd.expected_responses[1] = NULL;
  current_at_cmd.cb = AT_generic_cb;
}

void AT_generic_cb(bool success)
{
  if(success) {
    info("EVT_GSM_SUCCESS\n");
    push_event(EVT_GSM_SUCCESS);
  } else {
    info("EVT_GSM_FAILURE\n");
    push_event(EVT_GSM_FAILURE);
  }
}

void AT_send(char cmd)
{
  Serial.write(cmd);
  mySerial.write(cmd);
}

void AT_send(const char * cmd)
{
  Serial.print(cmd);
  mySerial.print(cmd);
}

void AT_send(const __FlashStringHelper* cmd)
{
  int i = 0;
  const char *ptr = (const char *) cmd;
  while (pgm_read_byte(ptr + i) != 0x00) {
    Serial.print((char)pgm_read_byte(ptr + i));
    mySerial.print((char)pgm_read_byte(ptr + i));
    i++;
  }
}

void AT_read()
{
  while(mySerial.available()) {
    char c = mySerial.read();
    receive_buff[receive_buff_index] = c;
    receive_buff[receive_buff_index+1] = 0;
    receive_buff_index++;
    if(c=='\n'){
      info("<<[");
      print_without_line_ending(receive_buff);
      info("]\n");
      int i = 0;
      int found = false;
      for(int i=0; i<MAX_EXPECTED_RESPONSES; i++){
        // Serial.println(current_at_cmd.expected_responses[i]);
        if(current_at_cmd.expected_responses[i]==NULL){
          continue;
        }
        if(0==strncmp(receive_buff, current_at_cmd.expected_responses[i], strlen(current_at_cmd.expected_responses[i]))){
          found = true;
          break;
        }
      }
      if(found) {
        void (*cb)(bool success) = current_at_cmd.cb;
        memset(&current_at_cmd, 0, sizeof(current_at_cmd));
        cb(true);
      } else {
        info("Dropping !\n");
      }
      receive_buff_index = 0;
    }
  }

  if (current_at_cmd.timeout_date>0) {
    if(millis()>current_at_cmd.timeout_date) {
      info("Timeout Occured\n");
      current_at_cmd.timeout_date = 0;
      void (*cb)(bool success) = current_at_cmd.cb;
      memset(&current_at_cmd, 0, sizeof(current_at_cmd));
      cb(false);
    }
  }
}