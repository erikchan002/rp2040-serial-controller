#include "gamepad.h"

void Gamepad::setup() {
  crc8 = CRC8();
  packetSerial.begin(115200);
  packetSerial.setPacketHandler(&onPacketReceived, this);
}

void Gamepad::read() { packetSerial.update(); }

void Gamepad::onPacketReceived(const uint8_t* buffer, size_t size) {
  if (size != PACKET_SIZE) {
    sendStatePacket();
    return;
  }
  crc8.restart();
  crc8.add(buffer, size);
  if (crc8.getCRC() != 0) {
    sendStatePacket();
    return;
  }

  size_t index = 0;

  memcpy(&state.dpad, buffer + index, sizeof(state.dpad));
  index += sizeof(state.dpad);

  memcpy(&state.buttons, buffer + index, sizeof(state.buttons));
  index += sizeof(state.buttons);

  memcpy(&state.aux, buffer + index, sizeof(state.aux));
  index += sizeof(state.aux);

  memcpy(&state.lx, buffer + index, sizeof(state.lx));
  index += sizeof(state.lx);

  memcpy(&state.ly, buffer + index, sizeof(state.ly));
  index += sizeof(state.ly);

  memcpy(&state.rx, buffer + index, sizeof(state.rx));
  index += sizeof(state.rx);

  memcpy(&state.ry, buffer + index, sizeof(state.ry));
  index += sizeof(state.ry);

  memcpy(&state.lt, buffer + index, sizeof(state.lt));
  index += sizeof(state.lt);

  memcpy(&state.rt, buffer + index, sizeof(state.rt));
  index += sizeof(state.rt);

  sendStatePacket();
}

void Gamepad::sendStatePacket() {
  uint8_t buffer[PACKET_SIZE];

  size_t index = 0;

  memcpy(buffer + index, &state.dpad, sizeof(state.dpad));
  index += sizeof(state.dpad);

  memcpy(buffer + index, &state.buttons, sizeof(state.buttons));
  index += sizeof(state.buttons);

  memcpy(buffer + index, &state.aux, sizeof(state.aux));
  index += sizeof(state.aux);

  memcpy(buffer + index, &state.lx, sizeof(state.lx));
  index += sizeof(state.lx);

  memcpy(buffer + index, &state.ly, sizeof(state.ly));
  index += sizeof(state.ly);

  memcpy(buffer + index, &state.rx, sizeof(state.rx));
  index += sizeof(state.rx);

  memcpy(buffer + index, &state.ry, sizeof(state.ry));
  index += sizeof(state.ry);

  memcpy(buffer + index, &state.lt, sizeof(state.lt));
  index += sizeof(state.lt);

  memcpy(buffer + index, &state.rt, sizeof(state.rt));
  index += sizeof(state.rt);

  packetSerial.send(buffer, PACKET_SIZE);
}