/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#define GAMEPAD_DEBOUNCE_MILLIS 5

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"
#include "pico/util/queue.h"
#include "tusb.h"
#include "class/hid/hid_device.h"
#include "gamepad.h"
#include "ws2812.pio.h"

uint32_t getMillis() { return to_ms_since_boot(get_absolute_time()); }

Gamepad gamepad(GAMEPAD_DEBOUNCE_MILLIS);
static InputMode inputMode;
queue_t gamepadQueue;

static inline void put_pixel(uint32_t pixel_grb)
{
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)(r) << 8) |
         ((uint32_t)(g) << 16) |
         (uint32_t)(b);
}

const int PIN_BUILTIN_RGB = 16;

void setup();
void loop();
void core1();

int main()
{
    setup();
    multicore_launch_core1(core1);

    while (1)
        loop();

    return 0;
}

void setup()
{

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, PIN_BUILTIN_RGB, 800000, true);

    gamepad.setup();

    gamepad.options.inputMode = INPUT_MODE_SWITCH;
    queue_init(&gamepadQueue, sizeof(Gamepad), 1);

    tusb_init();
}

void loop()
{
    static void *report;
    static const uint16_t reportSize = gamepad.getReportSize();
    static const uint32_t intervalMS = 1;
    static uint32_t nextRuntime = 0;
    static Gamepad snapshot;

    if (getMillis() - nextRuntime < 0)
        return;

    gamepad.read();
#if GAMEPAD_DEBOUNCE_MILLIS > 0
    gamepad.debounce();
#endif
    gamepad.hotkey();
    gamepad.process();
    report = gamepad.getReport();

    if (tud_suspended())
		tud_remote_wakeup();

    if (tud_hid_ready())
		tud_hid_report(0, report, reportSize);

    tud_task();

    if (queue_is_empty(&gamepadQueue))
    {
        memcpy(&snapshot, &gamepad, sizeof(Gamepad));
        queue_try_add(&gamepadQueue, &snapshot);
    }

    nextRuntime = getMillis() + intervalMS;
}

void core1()
{
    multicore_lockout_victim_init();

    while (1)
    {
        static Gamepad snapshot;

        if (queue_try_remove(&gamepadQueue, &snapshot))
        {
            // process add-on with &snapshot
        }

        // add-on loop
    }
}

/* USB HID Callbacks (Required) */

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
	// TODO: Handle the correct report type, if required
	(void)itf;
	(void)report_id;
	(void)report_type;
	(void)reqlen;

	uint8_t report_size = 0;
	SwitchReport switch_report;
	HIDReport hid_report;
    report_size = sizeof(SwitchReport);
    memcpy(buffer, &switch_report, report_size);

	return report_size;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
	(void) itf;

	// echo back anything we received from host
	tud_hid_report(report_id, buffer, bufsize);
}