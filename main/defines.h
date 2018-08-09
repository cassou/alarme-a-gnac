#ifndef _DEF_H_
#define _DEF_H_

#define REMOTE_PROTOCOL      (1)
#define REMOTE_ID_LEN        (20)
#define REMOTE_CMD_LEN       (4)
#define REMOTE_CMD_MASK      (0b1111)
#define REMOTE_BIT_LEN       (REMOTE_ID_LEN+REMOTE_CMD_LEN)

#define PHONE_NUMBER_MAX_LEN (15+1) //https://en.wikipedia.org/wiki/Telephone_numbering_plan
#define PHONE_NUMBERS_COUNT (3)
#define REMOTES_COUNT (5)
#define SENSORS_COUNT (10)
#define SENSOR_NAME_LEN (10+1) //including NULL terminator

enum sensor_type {
  SENSOR_DEACTIVATED, // does not trigger anything
  SENSOR_NOTIFY,      // does nor trigger an alarm, just send a sms
  SENSOR_TRIGGER,     // trigger the alarm if armed
  SENSOR_DELAY,       // trigger the alarm (if armed) after a delay
};

struct sensor_t {
  uint32_t id;
  enum sensor_type type;
  char name[SENSOR_NAME_LEN];
};

#define CONFIG_VERSION "0002"

struct config {
  char version[4];
  enum alarm_state alarm_state;
  uint32_t remotes[REMOTES_COUNT];
  char phone_numbers[PHONE_NUMBERS_COUNT][PHONE_NUMBER_MAX_LEN];
  struct sensor_t sensors[SENSORS_COUNT];
};

extern struct config config;

#endif
