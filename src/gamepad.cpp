#include "gamepad.h"

void Gamepad::setup() {
  crc8 = CRC8();
  packetSerial.begin(115200);
  packetSerial.setPacketHandler(&onPacketReceived, this);
}

void Gamepad::read() { packetSerial.update(); }

void Gamepad::onPacketReceived(const uint8_t* buffer, size_t size) {
  crc8.restart();
  crc8.add(buffer, size);
  if (crc8.getCRC() != 0) {
    return;
  }

  size_t index = 0;

  if (index + sizeof(state.dpad) > size) return;
  memcpy(&state.dpad, buffer + index, sizeof(state.dpad));
  index += sizeof(state.dpad);

  if (index + sizeof(state.buttons) > size) return;
  memcpy(&state.buttons, buffer + index, sizeof(state.buttons));
  index += sizeof(state.buttons);

  if (index + sizeof(state.aux) > size) return;
  memcpy(&state.aux, buffer + index, sizeof(state.aux));
  index += sizeof(state.aux);

  if (index + sizeof(state.lx) > size) return;
  memcpy(&state.lx, buffer + index, sizeof(state.lx));
  index += sizeof(state.lx);

  if (index + sizeof(state.ly) > size) return;
  memcpy(&state.ly, buffer + index, sizeof(state.ly));
  index += sizeof(state.ly);

  if (index + sizeof(state.rx) > size) return;
  memcpy(&state.rx, buffer + index, sizeof(state.rx));
  index += sizeof(state.rx);

  if (index + sizeof(state.ry) > size) return;
  memcpy(&state.ry, buffer + index, sizeof(state.ry));
  index += sizeof(state.ry);

  if (index + sizeof(state.lt) > size) return;
  memcpy(&state.lt, buffer + index, sizeof(state.lt));
  index += sizeof(state.lt);

  if (index + sizeof(state.rt) > size) return;
  memcpy(&state.rt, buffer + index, sizeof(state.rt));
  index += sizeof(state.rt);
}
