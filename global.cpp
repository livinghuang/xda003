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

void at_send_response(String response)
{
  BLEatService.sendResponse(response);
  UARTatService.sendResponse(response);
}

void add_at_commands()
{
  // 注册带参数的 "AT+INTV" 命令
  UARTatService.registerCommand("AT+INTV", [](String cmd, String param)
                                {
        if (param.length() > 0)
        {
          if (param == "?")
          {
            at_send_response("Interval time is " + String(params.uplinkIntervalSeconds) + " s.\r\n");
          }
          else
          {
            if (parseIntervalTime(param))
            {
              at_send_response("Interval time set to " + String(params.uplinkIntervalSeconds) + " s.\r\n");
            }else{
              at_send_response("ERROR: Invalid parameter for AT+INTV\r\n");
            }
          }
        }
        else
        {
          at_send_response("ERROR: Missing parameter for AT+INTV\r\n");
        } });
}
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