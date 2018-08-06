
void myPrintf(const char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}

void print_without_line_ending(const char * txt)
{
  int i = 0;
  while(txt[i]!=0){
    if(txt[i]!='\n' && txt[i]!='\r'){
      Serial.write(txt[i]);
    }
    i++;
  }
}