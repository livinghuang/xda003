#include "global.h"

bool parseIntervalTime(String param)
{
  // 解析参数为整数，设置定时器间隔数，并发送回响应
  int second = param.toInt();
  if ((second >= 15) && (second <= 604800)) // Interval time is between 15s and 604800s
  {
    global_system_data.uplinkIntervalSeconds = second;
    params.uplinkIntervalSeconds = second;
    storageSystemData(&global_system_data, sizeof(global_system_data));
    return true;
  }
  return false;
}

// void at_send_response(String response)
// {
// BLEatService.sendResponse(response);
// UARTatService.sendResponse(response);
// }

// void add_at_commands()
// {
// 注册带参数的 "AT+INTV" 命令
// UARTatService.registerCommand("AT+INTV", [](String cmd, String param)
//                               {
//       if (param.length() > 0)
//       {
//         if (param == "?")
//         {
//           at_send_response("Interval time is " + String(params.uplinkIntervalSeconds) + " s.\r\n");
//         }
//         else
//         {
//           if (parseIntervalTime(param))
//           {
//             at_send_response("Interval time set to " + String(params.uplinkIntervalSeconds) + " s.\r\n");
//           }else{
//             at_send_response("ERROR: Invalid parameter for AT+INTV\r\n");
//           }
//         }
//       }
//       else
//       {
//         at_send_response("ERROR: Missing parameter for AT+INTV\r\n");
//       } });
// }
void storage(void)
{
  // Get file system statistics
  size_t totalBytes = sqLittleFS.totalBytes();
  size_t usedBytes = sqLittleFS.usedBytes();
  size_t freeBytes = totalBytes - usedBytes;

  // Check if free space is below the threshold
  if (freeBytes < 100)
  {
    console.log(sqINFO, "Low storage space detected. Searching for the file with the smallest number...");

    // Variables to track the smallest file
    String smallestFile;
    int smallestNumber = INT32_MAX; // Initialize to a very large number

    // Scan the LittleFS root directory
    File root = sqLittleFS.open("/");
    if (!root)
    {
      console.log(sqERROR, "Failed to open root directory");
      return;
    }

    File file = root.openNextFile();
    while (file)
    {
      // Extract the numeric part of the file name
      String fileName = String(file.name());
      if (fileName.startsWith("/data") && fileName.endsWith(".txt"))
      {
        int fileNumber = fileName.substring(5, fileName.length() - 4).toInt(); // Extract number between "data" and ".txt"
        if (fileNumber < smallestNumber)
        {
          smallestNumber = fileNumber;
          smallestFile = fileName;
        }
      }
      file = root.openNextFile(); // Move to the next file
    }

    root.close();

    // Delete the file with the smallest number
    if (smallestFile.length() > 0)
    {
      if (fileSystem.deleteFile(smallestFile.c_str()))
      {
        console.log(sqINFO, "File with the smallest number deleted: " + smallestFile);
      }
      else
      {
        console.log(sqERROR, "Failed to delete file: " + smallestFile);
      }
    }
    else
    {
      console.log(sqINFO, "No files matching the pattern found.");
    }
  }

  // Append new data to the current file
  String filename = "/data" + String(global_system_data.storage_filename_counter / 200) + ".txt";
  // if (fileSystem.appendFile(filename.c_str(), String(global_sensor_data.pt100.whole)))
  // {
  //   console.log(sqINFO, "Data successfully appended to: " + filename);
  // }
  // else
  // {
  //   console.log(sqERROR, "Failed to append data to file: " + filename);
  // }
  global_system_data.storage_filename_counter++;
  storageSystemData(&global_system_data, sizeof(global_system_data));
}

void print_hex(const uint8_t *data, size_t len)
{
  for (size_t i = 0; i < len; i++)
  {
    if (data[i] < 0x10)
    {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void print_global_report_data()
{
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
}

#include <vector>

static const size_t MAX_VECTOR_BYTES = 256; // Define the maximum byte size
static std::vector<uint8_t> data_queue;

RTC_DATA_ATTR uint8_t vector_buffer[MAX_VECTOR_BYTES];
RTC_DATA_ATTR size_t vector_buffer_size = 0; // Store actual data size

void vector_to_buffer()
{
  if (!data_queue.empty())
  {
    vector_buffer_size = data_queue.size();
    std::copy(data_queue.begin(), data_queue.end(), vector_buffer);

    // Clear remaining buffer space to prevent old data issues
    std::fill(vector_buffer + vector_buffer_size, vector_buffer + MAX_VECTOR_BYTES, 0);
  }
  else
  {
    vector_buffer_size = 0; // Ensure no stale data remains
  }
}

void buffer_to_vector()
{
  if (data_queue.empty() && vector_buffer_size > 0)
  {
    data_queue.insert(data_queue.end(), vector_buffer, vector_buffer + vector_buffer_size);
  }
}

void put_data_to_vector(uint8_t *data, size_t len)
{
  if (data && len > 0)
  {
    // Ensure the total bytes do not exceed MAX_VECTOR_BYTES
    if (data_queue.size() + len > MAX_VECTOR_BYTES)
    {
      size_t excess = (data_queue.size() + len) - MAX_VECTOR_BYTES;
      data_queue.erase(data_queue.begin(), data_queue.begin() + excess); // Remove oldest bytes
    }

    data_queue.insert(data_queue.end(), data, data + len);
  }
}

size_t get_data_from_vector(uint8_t *data, size_t len)
{
  size_t data_size = std::min(len, data_queue.size());
  if (data_size > 0)
  {
    std::copy(data_queue.begin(), data_queue.begin() + data_size, data);
    data_queue.erase(data_queue.begin(), data_queue.begin() + data_size);
  }
  return data_size;
}

bool is_vector_empty()
{
  return data_queue.empty();
}

size_t get_vector_size()
{
  return data_queue.size(); // Returns the total byte size
}

void print_top_vector_data()
{
  if (!data_queue.empty())
  {
    Serial.print("Top data in vector: ");
    print_hex(data_queue.data(), data_queue.size());
  }
}
