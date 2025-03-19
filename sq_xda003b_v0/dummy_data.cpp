#include "dummy_data.h"

void get_dummy_battary_data(struct battery_data *battery_data)
{
  battery_data->power_mode = 0;
  battery_data->battery_level = 128;
}

void get_dummy_beacon_data(struct peripheral_readable_beacon_data *beacon_data)
{
  uint8_t uuid[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  beacon_data->rssi = -30;
  beacon_data->major = 0;
  beacon_data->minor = 0;
  memcpy(beacon_data->uuid, uuid, 8);
}

void get_dummy_hook_data(struct peripheral_hook_sensor_data *hook_data)
{
  uint8_t hook_id0[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
  uint8_t hook_id1[6] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
  static bool flag = false;
  flag = !flag;
  if (flag)
  {
    memcpy(hook_data->hook_id, hook_id0, 6);
  }
  else
  {
    memcpy(hook_data->hook_id, hook_id1, 6);
  }
  get_dummy_battary_data(&hook_data->hook_battery);
  hook_data->mode = 0;
  hook_data->status = 0;
  hook_data->reserved = 0;
}

void get_dummy_reed_switch_data(struct reed_switch_data *reed_switch_data)
{
  reed_switch_data->mode = 0;
  reed_switch_data->status = 0;
}

void get_dummy_sensor_data(struct peripheral_sensor_status *sensor_data)
{
  get_dummy_hook_data(&sensor_data->hooks[0]);
  get_dummy_hook_data(&sensor_data->hooks[1]);
  get_dummy_reed_switch_data(&sensor_data->reed_switch);
}

void get_dummy_report_data(struct report_data *report_data)
{
  get_dummy_battary_data(&report_data->battery_status);
  get_dummy_beacon_data(&report_data->biggest_rssi_readable_in_past_one_minutes_beacon_data);
  get_dummy_sensor_data(&report_data->sensor_status);
}