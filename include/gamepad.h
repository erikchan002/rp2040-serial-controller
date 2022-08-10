#include <MPG.h>

#include "CRC8.h"
#include "PacketSerial.h"
class Gamepad : public MPG {
 public:
  Gamepad(int debounceMS = 5) : MPG(debounceMS) {}

  void setup();
  void read();
  PacketSerial packetSerial;
  CRC8 crc8;

  void onPacketReceived(const uint8_t* buffer, size_t size);
  static void onPacketReceived(const void* sender, const uint8_t* buffer,
                               size_t size) {
    ((Gamepad*)sender)->onPacketReceived(buffer, size);
  }
};