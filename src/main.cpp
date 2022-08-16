
#define GAMEPAD_DEBOUNCE_MILLIS 5

#include "class/hid/hid_device.h"
#include "gamepad.h"
#include "tusb.h"

uint32_t getMillis() { return to_ms_since_boot(get_absolute_time()); }

Gamepad gamepad(GAMEPAD_DEBOUNCE_MILLIS);

void setup();
void loop();

void setup() {
  gamepad.setup();

  gamepad.options.inputMode = INPUT_MODE_SWITCH;
  gamepad.hasLeftAnalogStick = true;
  gamepad.hasRightAnalogStick = true;

  tusb_init();
}

void loop() {
  static void *report;
  static const uint16_t reportSize = gamepad.getReportSize();
  static const uint32_t intervalMS = 1;
  static uint32_t nextRuntime = 0;
  static Gamepad snapshot;

  if (getMillis() - nextRuntime < 0) return;

  gamepad.read();
#if GAMEPAD_DEBOUNCE_MILLIS > 0
  gamepad.debounce();
#endif
  gamepad.hotkey();
  gamepad.process();
  report = gamepad.getReport();

  if (tud_suspended()) tud_remote_wakeup();

  if (tud_hid_ready()) tud_hid_report(0, report, reportSize);

  tud_task();

  nextRuntime = getMillis() + intervalMS;
}

/* USB HID Callbacks (Required) */

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  uint8_t report_size = 0;
  SwitchReport switch_report;
  HIDReport hid_report;
  report_size = sizeof(SwitchReport);
  memcpy(buffer, &switch_report, report_size);

  return report_size;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  // echo back anything we received from host
  tud_hid_report(report_id, buffer, bufsize);
}