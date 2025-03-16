#include "bsp.h"
#include "siliqs_esp32.h"
#include "esp_sleep.h"
#include "sensors/ptc_measurement.h"
#include "sensors/battery_measurement.h"

#include "global.h"
#include "led.h"
#include "battery.h"
#include "dummy_data.h"
#include "commands.h"

#define SYSTEM_CYCLE_TIME 60 // unit  : second

// 创建 BLEATCommandService 实例
BLEATCommandService BLEatService;
// 创建 UARTATCommandService 实例，使用 Serial 作为通信接口
UARTATCommandService UARTatService;
/**                                                                                     \
 * @brief setup 函数，用于初始化系统                                          \
 *                                                                                      \
 * 该函数首先调用 siliqs_esp32_setup() 函数来初始化 ESP32 主板。 \
 */

#define LORA_DIO1 3
#define LORA_BUSY 4
#define LORA_NRST 5
#define LORA_MISO 6
#define LORA_MOSI 7
#define LORA_NSS 8
#define LORA_SCK 10

lorawan_params_settings params = {
    .DIO1 = LORA_DIO1,                                                                                            // DIO1
    .BUSY = LORA_BUSY,                                                                                            // BUSY
    .NRST = LORA_NRST,                                                                                            // reset
    .MISO = LORA_MISO,                                                                                            // MISO
    .MOSI = LORA_MOSI,                                                                                            // MOSI
    .SCK = LORA_SCK,                                                                                              // SCK
    .NSS = LORA_NSS,                                                                                              // NSS
    .uplinkIntervalSeconds = 60,                                                                                  // Unit: second, upline interval time
    .ADR = true,                                                                                                  // use ADR or not
    .DR = 5,                                                                                                      // Data Rate when start, if ADR is true, this will be tuned automatically
    .DutyCycleFactor = 1250,                                                                                      // Duty Cycle = 1 / (DutyCycleFactor) , if 0, disable. In EU law, Duty Cycle should under 1%
    .DwellTime = 400,                                                                                             // Unit: ms, Dwell Time to limit signal airtime in single channel, In US/AU law,Dwell Time under 400ms
    .OTAA = false,                                                                                                // OTAA or ABP
    .LORAWAN_1_1 = false,                                                                                         // LORAWAN 1.1 or 1.0
    .JOINEUI = 0x0000000000000000,                                                                                // Join EUI
    .DEVEUI = 0x5588888888888855,                                                                                 // DEVEUI, if OTAA, DEVEUI will used. if ABP, DEVEUI will be ignored
    .APPxKEY = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55},  // if OTAA, APPxKEY = APPKEY, if ABP, APPxKEY = APPSKEY
    .NWKxKEY = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55},  // if OTAA, NWKxKEY = NULL use lorawan v1.0.x, if ABP, APPxKEY = NWKSKEY
    .DEVADDR = 0x55555555,                                                                                        // if ABP, DEVADDR, if OTAA, DEVADDR will be ignored
    .FNWKSINT = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, // FNWKSINT, if lorawan v1.0.x, set as NULL, if lorawan v1.1.x
    .SNWKSINT = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, // SNWKSINT, if lorawan v1.0.x, set as NULL, if lorawan v1.1.x
};
LoRaWanService lorawan(&params);

struct system_data global_system_data;
struct report_data global_report_data;

uint32_t time_start = 0;
void setup()
{
  siliqs_esp32_setup();

   while (1)
  {
    command_test();
    delay(1000);
  }

  // fetch_battary(1);

  // if (global_sensor_data.battery_voltage < 3.32)
  // {
  //   blink_led(1);
  //   lorawan.sleep(LORAWAN_SLEEP_IN_RADIO_OFF);
  //   gotoSleep(60000);
  //   while (1)
  //     ;
  // }

  lorawan.begin(true);
  time_start = millis();
  if (readSystemData(&global_system_data, sizeof(global_system_data)))
  {
    if (global_system_data.uplinkIntervalSeconds > 86400)
    {
      global_system_data.uplinkIntervalSeconds = 180;
      global_system_data.storage_filename_counter = 0;
    }

    Serial.print("System data loaded successfully.\r\n");
    params.uplinkIntervalSeconds = global_system_data.uplinkIntervalSeconds;
  }
  else
  {
    Serial.print("No system data found.\r\n");
    global_system_data.uplinkIntervalSeconds = params.uplinkIntervalSeconds;
    global_system_data.storage_filename_counter = 0;
    storageSystemData(&global_system_data, sizeof(global_system_data));
  }
  Serial.print("Interval time set to " + String(params.uplinkIntervalSeconds) + " s.\r\n");
  Serial.print("Storage filename counter is " + String(global_system_data.storage_filename_counter) + " \r\n");
  Serial.flush();
  if (bootCount == 1)
  {
    Serial.println("AT Command Service is starting...");
    add_at_commands();
    BLEatService.startTask();
    UARTatService.startTask();
    pinMode(pVext, OUTPUT);
    digitalWrite(pVext, LOW);
    delay(1000);
  }
}

void loop()
{
  Serial.println("boot count: " + String(bootCount));
  if (bootCount == 1)
  {
    send_data_to_server();
    // wait for seconds for user enter at command mode
    blink_led(60);
    while (nimbleService.deviceConnected)
    {
      vTaskDelay(100);
    }
  }
  else
  {
    Serial.println("next uplink count down: " + String((params.uplinkIntervalSeconds / SYSTEM_CYCLE_TIME) - bootCount % (params.uplinkIntervalSeconds / SYSTEM_CYCLE_TIME)));
    if ((bootCount % (params.uplinkIntervalSeconds / SYSTEM_CYCLE_TIME)) == 0)
    {
      // delay(random(0, 1000));
      if (1) // (is_temperature_valid())
      {
        send_data_to_server();
      }
    }
  }
  uint32_t used_time = millis() - time_start;
  Serial.println("used time: " + String(used_time) + " ms");

  uint32_t sleep_time = (SYSTEM_CYCLE_TIME * 1000 - used_time);
  Serial.println("sleep time: " + String(sleep_time) + " millisecond");
  // go to sleep for a while
  gotoSleep(sleep_time);
}

void send_data_to_server()
{
  lorawan.begin();
  uint8_t updata[sizeof(global_report_data)];
  static uint8_t fport = 10;
  uint8_t downbuffer[255];
  size_t downlen = 0;
  bool isConfirmed = true;
  lorawan.set_battery_level(global_report_data.battery_status.battery_level);

  get_dummy_report_data(&global_report_data);

  Serial.println("=====================================");
  Serial.println("Battery level: " + String(global_report_data.battery_status.battery_level));
  Serial.println("Battery power mode: " + String(global_report_data.battery_status.power_mode) + "=> 0: normal , 1: power saving , 2: charging");
  Serial.println("=====================================");
  Serial.print("Beacon ID: ");
  print_hex(global_report_data.biggest_rssi_readable_in_past_one_minutes_beacon_data.uuid, sizeof(global_report_data.biggest_rssi_readable_in_past_one_minutes_beacon_data.uuid));
  Serial.println("Beacon RSSI: " + String(global_report_data.biggest_rssi_readable_in_past_one_minutes_beacon_data.rssi));
  Serial.println("Beacon Major: " + String(global_report_data.biggest_rssi_readable_in_past_one_minutes_beacon_data.major));
  Serial.println("Beacon Minor: " + String(global_report_data.biggest_rssi_readable_in_past_one_minutes_beacon_data.minor));
  Serial.println("=====================================");
  Serial.println("Hook 0 ID: ");
  print_hex(global_report_data.sensor_status.hooks[0].hook_id, sizeof(global_report_data.sensor_status.hooks[0].hook_id));
  Serial.println("Hook 0 Mode: " + String(global_report_data.sensor_status.hooks[0].mode) + "=>  0: normal 1: un-know");
  Serial.println("Hook 0 Status: " + String(global_report_data.sensor_status.hooks[0].status) + "=>  0: normal 1: alarm");
  Serial.println("Hook 0 Battery Level: " + String(global_report_data.sensor_status.hooks[0].hook_battery.battery_level));
  Serial.println("-------------------------------------");
  Serial.println("Hook 1 ID: ");
  print_hex(global_report_data.sensor_status.hooks[1].hook_id, sizeof(global_report_data.sensor_status.hooks[1].hook_id));
  Serial.println("Hook 1 Mode: " + String(global_report_data.sensor_status.hooks[1].mode) + "=>  0: normal 1: un-know");
  Serial.println("Hook 1 Status: " + String(global_report_data.sensor_status.hooks[1].status) + "=>  0: normal 1: alarm");
  Serial.println("Hook 1 Battery Level: " + String(global_report_data.sensor_status.hooks[1].hook_battery.battery_level));
  Serial.println("=====================================");
  Serial.println("Reed Switch Mode: " + String(global_report_data.sensor_status.reed_switch.mode) + "=>  0: normal 1: un-know");
  Serial.println("Reed Switch Status: " + String(global_report_data.sensor_status.reed_switch.status) + "=>  0: normal 1: alarm");
  Serial.println("=====================================");
  memcpy(updata, &global_report_data, sizeof(global_report_data));

  lorawan.send_and_receive(updata, sizeof(updata), fport, downbuffer, &downlen, isConfirmed);
  lorawan.sleep(LORAWAN_SLEEP_IN_RADIO_OFF); // use radio off, spi ended

  if (downlen > 0)
  {
    Serial.print("Downlink: ");
    for (int i = 0; i < downlen; i++)
    {
      if (downbuffer[i] < 0x10)
      {
        Serial.print("0");
      }
      Serial.print(downbuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
