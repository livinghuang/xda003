#pragma once
#include "global.h"
#include <stdint.h>
#include <iostream>
#include <memory>
#include <cstring>
#include <algorithm>
#define COMAND_ID_T uint16_t
#define SIZE_OF_COMMAND_ID sizeof(COMAND_ID_T) // 2 bytes

// Downlink Packet Structure
struct downlink_packet
{
  COMAND_ID_T command_id;
  uint8_t params[48];
};

// Command ID Enum
enum command_id : COMAND_ID_T
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
  uint8_t data_length = sizeof(echo_data.echo);

public:
  void parse(const uint8_t *params, size_t length) override
  {
    data_length = length - SIZE_OF_COMMAND_ID;
    memcpy(echo_data.echo, params, length);
  }

  void execute() override
  {
    if (data_length == 0)
    {
      Serial.println("Echo Data: (Empty)");
    }
    else
    {
      Serial.print("Echo Data: ");
      print_hex(echo_data.echo, data_length);
      put_data_to_vector(echo_data.echo, data_length);
      print_top_vector_data();
    }
  }
};

// Command Factory
class CommandFactory
{
public:
  static std::unique_ptr<Command> createCommand(COMAND_ID_T command_id)
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
  void print_invalid_command(COMAND_ID_T command_id)
  {
    Serial.println("Invalid command ID: " + String(command_id));
  }

public:
  DownlinkCommand(const downlink_packet *packet, size_t length)
  {
    command = CommandFactory::createCommand(packet->command_id);

    if (command)
    {
      command->parse(packet->params, length);
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