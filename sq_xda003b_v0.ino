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
#include "ble.h"

#define SYSTEM_CYCLE_TIME 10 // unit  : second

// 创建 BLEATCommandService 实例
// BLEATCommandService BLEatService;
// 创建 UARTATCommandService 实例，使用 Serial 作为通信接口
// UARTATCommandService UARTatService;
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
    .uplinkIntervalSeconds = 10,                                                                                  // Unit: second, upline interval time
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
  siliqs_esp32_setup(SQ_INFO);
  buffer_to_vector();
  ble_init();
  // fetch_battary(1);

  // if (global_sensor_data.battery_voltage < 3.32)
  // {
  //   blink_led(1);
  //   lorawan.sleep(LORAWAN_SLEEP_IN_RADIO_OFF);
  //   gotoSleep(60000);
  //   while (1)
  //     ;
  // }

  time_start = millis();
  // if (readSystemData(&global_system_data, sizeof(global_system_data)))
  // {
  //   if (global_system_data.uplinkIntervalSeconds > 86400)
  //   {
  //     global_system_data.uplinkIntervalSeconds = 180;
  //     global_system_data.storage_filename_counter = 0;
  //   }

  //   Serial.print("System data loaded successfully.\r\n");
  //   params.uplinkIntervalSeconds = global_system_data.uplinkIntervalSeconds;
  // }
  // else
  // {
  //   Serial.print("No system data found.\r\n");
  //   global_system_data.uplinkIntervalSeconds = params.uplinkIntervalSeconds;
  //   global_system_data.storage_filename_counter = 0;
  //   storageSystemData(&global_system_data, sizeof(global_system_data));
  // }
  Serial.print("Interval time set to " + String(params.uplinkIntervalSeconds) + " s.\r\n");
  // Serial.print("Storage filename counter is " + String(global_system_data.storage_filename_counter) + " \r\n");
  Serial.flush();

  // if (bootCount == 1)
  // {
  //   Serial.println("AT Command Service is starting...");
  //   add_at_commands();
  //   BLEatService.startTask();
  //   UARTatService.startTask();
  //   pinMode(pVext, OUTPUT);
  //   digitalWrite(pVext, LOW);
  //   delay(1000);
  //   Serial.println("boot count: " + String(bootCount));
  //   send_data_to_server();
  //   // wait for seconds for user enter at command mode
  //   blink_led(30);
  //   while (nimbleService.deviceConnected)
  //   {
  //     vTaskDelay(100);
  //   }
  //   gotoSleep(1000);
  // }
  // // 初始化 NimBLE 服务
  // struct set_scan_params scan_params;
  // scan_params.scanTime = 0; // Scan time
  // scan_params.name = "";    // Device name
  // scan_params.address = ""; // MAC address
  // scan_params.scanActive = false;
  // scan_params.scanInterval = 1000; // Scan interval, unit: ms
  // scan_params.scanWindow = 100;    // Scan window, unit: ms
  // scan_params.scanMaxResults = 20; // Maximum number of results, 0 means no storage of results

  // nimbleService.init();
  // nimbleService.startScanDevices(scan_params);
  // uint32_t time = millis();
  // while (1) //
  // {
  //   if ((millis() - time > 8000))
  //   {
  //     time = millis();
  //     NimBLEScanResults scanResults = pBLEScan->getResults();
  //     Serial.print("Scan done!\n");
  //     Serial.print("Found ");
  //     Serial.print(scanResults.getCount());
  //     Serial.print(" device(s).\n");
  //     for (int i = 0; i < scanResults.getCount(); i++)
  //     {
  //       NimBLEAdvertisedDevice device = scanResults.getDevice(i);
  //       Serial.printf("Device %d:\n  Name: %s\n  Address: %s\n  RSSI: %d\n",
  //                     i + 1, device.getName().empty() ? "Unnamed" : device.getName().c_str(),
  //                     device.getAddress().toString().c_str(), device.getRSSI());
  //       // Check for Manufacturer Data (which may contain iBeacon UUID, Major, Minor)
  //       if (device.haveManufacturerData())
  //       {
  //         std::string manufacturerData = device.getManufacturerData();
  //         Serial.print("  Manufacturer Data: ");
  //       }
  //     }
  //   }
  // }

  // nimbleService.printDiscoveredDevices();
}

void loop()
{
  if ((bootCount % 3) == 0)
  {
    // queue the data to vector
    get_dummy_report_data(&global_report_data);
    print_global_report_data();
    put_data_to_vector((uint8_t *)&global_report_data, sizeof(global_report_data));
  }
  // send data to server
  send_data_to_server();

  vector_to_buffer();

  while (is_ble_working())
  {
    vTaskDelay(100);
  }

  Serial.println("close BLE");
  ble_close();

  uint32_t sleep_time = 1000;
  uint32_t used_time = millis() - time_start;
  Serial.println("used time: " + String(used_time) + " ms");
  if (used_time > SYSTEM_CYCLE_TIME * 1000)
  {
    Serial.println("Warning: used time is more than " + String(SYSTEM_CYCLE_TIME) + " second");
  }
  else
  {
    sleep_time = (SYSTEM_CYCLE_TIME * 1000 - used_time);
  }
  Serial.println("sleep time: " + String(sleep_time) + " millisecond");
  Serial.flush();
  // go to sleep for a while
  gotoSleep(sleep_time);
}

void send_data_to_server()
{
  if (is_vector_empty())
  {
    return; // No data to send
  }
  lorawan.begin(true);

  static constexpr uint8_t fport = 10;
  uint8_t downbuffer[255];
  size_t downlen = 0;
  constexpr bool isConfirmed = true;

  lorawan.set_battery_level(global_report_data.battery_status.battery_level);

  // Extract data from queue and send
  uint8_t updata[255];
  size_t data_size = get_data_from_vector(updata, sizeof(updata));

  lorawan.send_and_receive(updata, data_size, fport, downbuffer, &downlen, isConfirmed);
  lorawan.sleep(LORAWAN_SLEEP_IN_RADIO_OFF); // Put LoRa module into sleep mode

  if (downlen > 0)
  {
    Serial.print("Downlink: ");
    print_hex(downbuffer, downlen);

    downlink_packet packet;
    memcpy(&packet, downbuffer, std::min(downlen, sizeof(packet)));

    DownlinkCommand cmd(&packet, std::min(downlen, sizeof(packet)));
    cmd.execute();
  }
}
