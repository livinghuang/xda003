#pragma once
#include "global.h"
#include <stdint.h>
#include <iostream>
#include <memory>
#include <cstring>
#include <algorithm>

// Downlink Packet Structure
struct downlink_packet
{
  uint8_t command_id;
  uint8_t params[48];
};

// Command ID Enum
enum command_id
{
  CMD_ECHO = 0,
  CMD_SET_INTERVAL = 1,
  CMD_BIND_HOOK = 2,
  CMD_ADD_BEACONS = 3,
  CMD_REMOVE_BEACONS = 4,
  CMD_SECURITY_MODE = 5,
  CMD_POWER_MANAGEMENT = 6,
  CMD_FACTORY_RESET = 7,
  CMD_FIRMWARE_UPDATE = 8,
  CMD_EMERGENCY_TRANSMISSION = 9,
  CMD_DEEP_SLEEP = 10,
  CMD_ALARM_TEST = 11
};

// Abstract Base Command Class
class Command
{
public:
  virtual ~Command() {}
  virtual void parse(const uint8_t *params, size_t length) = 0;
  virtual void execute() = 0;
};

// Command 0: Echo Response Test
struct cmd_echo
{
  uint8_t echo[48]; // Maximum 48 bytes, device will echo the same content
};

class EchoCommand : public Command
{
private:
  struct cmd_echo echo_data;

public:
  void parse(const uint8_t *params, size_t length) override
  {
    // Clear buffer to avoid garbage data
    memset(echo_data.echo, 0, sizeof(echo_data.echo));

    // Copy only up to the available length
    size_t copy_len = std::min(static_cast<size_t>(sizeof(echo_data.echo)), length);
    memcpy(echo_data.echo, params, copy_len);
  }

  void execute() override
  {
    // Check if all bytes are 0
    bool is_empty = std::all_of(std::begin(echo_data.echo), std::end(echo_data.echo),
                                [](uint8_t byte)
                                { return byte == 0; });

    if (is_empty)
    {
      Serial.println("Echo Data: (Empty)");
    }
    else
    {
      Serial.print("Echo Data: ");
      print_hex(echo_data.echo, sizeof(echo_data.echo));
    }
  }
};

// Command Factory
class CommandFactory
{
public:
  static std::unique_ptr<Command> createCommand(uint8_t command_id)
  {
    switch (command_id)
    {
    case CMD_ECHO:
      return std::make_unique<EchoCommand>();
    default:
      return nullptr;
    }
  }
};

// DownlinkCommand Class
class DownlinkCommand
{
private:
  std::unique_ptr<Command> command;
  void print_invalid_command(uint8_t command_id);

public:
  DownlinkCommand(const downlink_packet *packet)
  {
    command = CommandFactory::createCommand(packet->command_id);

    if (command)
    {
      command->parse(packet->params, sizeof(packet->params));
    }
    else
    {
      print_invalid_command(packet->command_id);
    }
  }

  void execute()
  {
    if (command)
    {
      command->execute();
    }
    else
    {
      Serial.println("Invalid command, cannot execute.");
    }
  }
};

int command_test();