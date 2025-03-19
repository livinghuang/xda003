#include "commands.h"

// Main Function for Testing
int command_test()
{
  downlink_packet packet;
  packet.command_id = CMD_ECHO;
  uint8_t test_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21, 0xFF, 0xAB}; // "Hello!" + binary
  memcpy(packet.params, test_data, sizeof(test_data));

  DownlinkCommand cmd(&packet, SIZE_OF_COMMAND_ID + sizeof(test_data));
  cmd.execute();

  return 0;
}
