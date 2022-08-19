
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

#define USB_PID 0x0092
#define USB_VID 0x0F0D
#define USB_BCD 0x0200

tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = USB_BCD,

    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and
    // protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01,
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *)&desc_device;
}

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
  return switch_report_descriptor;
}

enum {
  ITF_NUM_HID,
  ITF_NUM_CDC_CCI,
  ITF_NUM_CDC_DCI,
  ITF_NUM_TOTAL,
};

#define CONFIG_TOTAL_LEN \
  (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN + TUD_CDC_DESC_LEN)

#define EPNUM_HID_IN 0x81
#define EPNUM_HID_OUT 0x02

#define EPNUM_CDC_NOTIF 0x83
#define EPNUM_CDC_OUT 0x04
#define EPNUM_CDC_IN 0x85

uint8_t const desc_fs_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),

    // Interface number, string index, protocol, report descriptor len, EP OUT &
    // IN address, size & polling interval
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                             sizeof(switch_report_descriptor), EPNUM_HID_OUT,
                             EPNUM_HID_IN, CFG_TUD_HID_EP_BUFSIZE, 1),

    // Interface number, string index, EP notification address and size, EP data
    // address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_CCI, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT,
                       EPNUM_CDC_IN, 64),
};

#if TUD_OPT_HIGH_SPEED
// Per USB specs: high speed capable device must report device_qualifier and
// other_speed_configuration

uint8_t const desc_hs_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),

    // Interface number, string index, protocol, report descriptor len, EP OUT &
    // IN address, size & polling interval
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                             sizeof(switch_report_descriptor), EPNUM_HID_OUT,
                             EPNUM_HID_IN, CFG_TUD_HID_EP_BUFSIZE, 1),

    // Interface number, string index, EP notification address and size, EP data
    // address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_CCI, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT,
                       EPNUM_CDC_IN, 512),
};

// device qualifier is mostly similar to device descriptor since we don't change
// configuration based on speed
tusb_desc_device_qualifier_t const desc_device_qualifier = {
    .bLength = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB = USB_BCD,

    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .bNumConfigurations = 0x01,
    .bReserved = 0x00,
};

// Invoked when received GET DEVICE QUALIFIER DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete. device_qualifier descriptor describes
// information about a high-speed capable device that would change if the device
// were operating at the other speed. If not highspeed capable stall this
// request.
uint8_t const *tud_descriptor_device_qualifier_cb(void) {
  return (uint8_t const *)&desc_device_qualifier;
}

// Invoked when received GET OTHER SEED CONFIGURATION DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete Configuration descriptor in the other speed
// e.g if high speed then this is for full speed and vice versa
uint8_t const *tud_descriptor_other_speed_configuration_cb(uint8_t index) {
  (void)index;  // for multiple configurations

  // if link speed is high return fullspeed config, and vice versa
  return (tud_speed_get() == TUSB_SPEED_HIGH) ? desc_fs_configuration
                                              : desc_hs_configuration;
}

#endif  // highspeed

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void)index;  // for multiple configurations

#if TUD_OPT_HIGH_SPEED
  // Although we are highspeed, host may be fullspeed.
  return (tud_speed_get() == TUSB_SPEED_HIGH) ? desc_hs_configuration
                                              : desc_fs_configuration;
#else
  return desc_fs_configuration;
#endif
}