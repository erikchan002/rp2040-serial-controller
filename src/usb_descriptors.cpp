
#include <wchar.h>

#include "GamepadDescriptors.h"
#include "tusb.h"

static const char string_manufacturer[] = "erikchan002";
static const char string_product[] = "Serial Controller";
static const char string_serial_number[] = "00000001";

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  uint16_t size = 0;
  switch (index) {
    case 1:
      return convertStringDescriptor(&size, string_manufacturer,
                                     strlen(string_manufacturer));
    case 2:
      return convertStringDescriptor(&size, string_product,
                                     strlen(string_product));
    case 3:
      return convertStringDescriptor(&size, string_serial_number,
                                     strlen(string_serial_number));
    default:
      return getStringDescriptor(&size, INPUT_MODE_SWITCH, index);
  }
}

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
  return switch_device_descriptor;
}

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
  (void)itf;
  return switch_report_descriptor;
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  return switch_configuration_descriptor;
}
