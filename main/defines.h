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


struct phone_config {
  char numbers[PHONE_NUMBERS_COUNT][PHONE_NUMBER_MAX_LEN];
};
struct remote_config {
  uint32_t remotes[REMOTES_COUNT];
};
struct sensor_config {
  struct sensor_t sensors[SENSORS_COUNT];
};

#define CONFIG_VERSION "0005"

#define VERSION_CONFIG_OFFSET (0)
#define VERSION_CONFIG_SIZE (4)

#define STATE_CONFIG_OFFSET (VERSION_CONFIG_OFFSET + VERSION_CONFIG_SIZE)
#define STATE_CONFIG_SIZE (sizeof(enum alarm_state))

#define PHONE_CONFIG_OFFSET (STATE_CONFIG_OFFSET + STATE_CONFIG_SIZE)
#define PHONE_CONFIG_SIZE (sizeof(struct phone_config))

#define REMOTE_CONFIG_OFFSET (PHONE_CONFIG_OFFSET + PHONE_CONFIG_SIZE)
#define REMOTE_CONFIG_SIZE (sizeof(struct remote_config))

#define SENSOR_CONFIG_OFFSET (REMOTE_CONFIG_OFFSET + REMOTE_CONFIG_SIZE)
#define SENSOR_CONFIG_SIZE (sizeof(struct sensor_config))


#endif
