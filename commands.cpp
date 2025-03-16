#include "commands.h"
// Main Function for Testing
int command_test()
{
  downlink_packet packet;
  packet.command_id = CMD_ECHO;
  uint8_t test_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21, 0xFF, 0xAB}; // "Hello!" + binary
  memcpy(packet.params, test_data, sizeof(test_data));

  DownlinkCommand cmd(&packet);
  cmd.execute();

  return 0;
}
// Function for safer Serial printing
void DownlinkCommand::print_invalid_command(uint8_t command_id)
{
#ifdef SERIAL_PRINTF_SUPPORTED
  Serial.printf("Invalid command ID: %u\n", command_id);
#else
  Serial.print("Invalid command ID: ");
  Serial.println(command_id);
#endif
}