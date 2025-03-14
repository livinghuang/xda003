#pragma once
#include "siliqs_esp32.h"
// Define system states
enum SystemState
{
  SLEEP_MODE,
  BLE_SCAN_MODE,
  UPLINK_MODE
};

struct battery_data // 2 bytes
{
  uint8_t power_mode;    // 0: normal , 1: power saving , 2: charging(under test if it could be detect or not)  ,1 byte
  uint8_t battery_level; // 0-255 ,1 byte
};

struct peripheral_readable_beacon_data // 16 bytes
{
  uint8_t uuid[8];   // 8 bytes
  int16_t rssi;      // 2 bytes
  uint16_t major;    // 2 bytes
  uint16_t minor;    // 2 bytes
  uint16_t reserved; // fill 0, 2 bytes
};

struct peripheral_hook_sensor_data // 12 bytes (aligned)
{
  uint8_t hook_id[6];
  uint8_t mode;   // 0: normal 1: un-know ( if device is not in dangerous zone, we may by detect reed switch ,it should be un-know)
  uint8_t status; // 0: normal 1: alarm
  uint16_t reserved;
  struct battery_data hook_battery;
};

struct reed_switch_data // 2 bytes
{
  uint8_t mode;   // 0: normal 1: un-know ( if device is not in dangerous zone, we may by detect reed switch ,it should be un-know)
  uint8_t status; // 0: normal 1: alarm
};

struct peripheral_sensor_status // 26 bytes
{
  struct peripheral_hook_sensor_data hooks[2]; // 24 bytes
  struct reed_switch_data reed_switch;         // 2 bytes
};

struct report_data // 46 bytes
{
  struct battery_data battery_status;                                                           // 2 bytes
  struct peripheral_readable_beacon_data biggest_rssi_readable_in_past_one_minutes_beacon_data; // 16 bytes
  struct peripheral_sensor_status sensor_status;                                                // 26 bytes
};

struct system_data
{
  uint32_t uplinkIntervalSeconds;
  uint32_t storage_filename_counter;
};

extern struct report_data global_report_data;
extern struct system_data global_system_data;
extern lorawan_params_settings params;

extern BLEATCommandService BLEatService;
extern UARTATCommandService UARTatService;

bool parseIntervalTime(String param);
void at_send_response(String response);
void add_at_commands();

void blink_led(int times);
void storage(void);

void print_hex(const uint8_t *data, size_t len);