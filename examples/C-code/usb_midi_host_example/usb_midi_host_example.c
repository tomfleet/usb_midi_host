/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 rppicomidi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/**
 * This demo program is designed to test the USB MIDI Host driver for a single USB
 * MIDI device connected to the USB Host port. It sends to the USB MIDI device the
 * sequence of half-steps from B-flat to D whose note numbers correspond to the
 * transport button LEDs on a Mackie Control compatible control surface. It also
 * prints to a UART serial port console the messages received from the USB MIDI device.
 *
 * This program works with a single USB MIDI device connected via a USB hub, but it
 * does not handle multiple USB MIDI devices connected at the same time.
 */
//#include "midi_handler.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_midi_host.h"
#include "midi_handler.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"


//GPIO7is USB switch
#define USB_SWITCH 7

#ifdef RASPBERRYPI_PICO_W
// The Board LED is controlled by the CYW43 WiFi/Bluetooth module
#include "pico/cyw43_arch.h"
#endif

//#define SYSEX_BUFFER_SIZE 256

//uint8_t sysex_buffer[SYSEX_BUFFER_SIZE];
//uint16_t sysex_index = 0;
//bool sysex_in_progress = false;

static uint8_t midi_dev_addr = 0;

    repeating_timer_t timer;


bool timer_callback(repeating_timer_t *rt) {
    // Read the status of USB_SWITCH GPIO
    gpio_set_dir(USB_SWITCH, GPIO_IN);
    bool level = gpio_get(USB_SWITCH);
    printf("Timer Callback: USB_SWITCH level read as %d\n", level);

    // Change the mode to output and drive it to the opposite level
    gpio_set_dir(USB_SWITCH, GPIO_OUT);
    gpio_put(USB_SWITCH, !level);
    printf("Timer Callback: USB_SWITCH level set to %d\n", !level);

    // Sleep for 1 second
    sleep_ms(1000);

    // Change the mode back to input
    gpio_set_dir(USB_SWITCH, GPIO_IN);
    printf("Timer Callback: USB_SWITCH mode set back to input\n");

    return true; // Keep the timer running
}
static void blink_led(void)
{
    static absolute_time_t previous_timestamp = {0};

    static bool led_state = false;

    absolute_time_t now = get_absolute_time();
    
    int64_t diff = absolute_time_diff_us(previous_timestamp, now);
    if (diff > 1000000) {
#ifdef RASPBERRYPI_PICO_W
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
#else
        board_led_write(led_state);
#endif
        led_state = !led_state;
        previous_timestamp = now;
    }
}

// Define the byte array
uint8_t midi_sysex_getDevice[] = {0xF0, 0x7E, 0x00, 0x06, 0x01, 0xF7};
void send_midi_sysex_message(uint8_t dev_addr, uint8_t cable_num, uint8_t* midi_sysex_message, uint16_t length) {
    // Send the SysEx message
    tuh_midi_stream_write(dev_addr, cable_num, midi_sysex_message, length);
    tuh_midi_stream_flush(dev_addr);
}

// void send_get_all_button_states_message(uint8_t dev_addr, uint8_t cable_num, uint8_t product_id) {
//     uint8_t message[] = {
//         0xF0, 0x47, 0x31, product_id, 0x40, 0x00, 0x00, 0xF7
//     };
//     tuh_midi_stream_write(dev_addr, cable_num, message, sizeof(message));
//     tuh_midi_stream_flush(dev_addr);
// }
static void send_next_note(bool connected)
{
    static uint8_t first_note = 0x5b; // Mackie Control rewind
    static uint8_t last_note = 0x5f; // Mackie Control stop
    static uint8_t message[6] = {
        0x90, 0x5f, 0x00,
        0x90, 0x5b, 0x7f,
    };
    // toggle NOTE On, Note Off for the Mackie Control channels 1-8 REC LED
    const uint32_t interval_ms = 1000;
    static uint32_t start_ms = 0;

    // device must be attached and have at least one endpoint ready to receive a message
    if (!connected || tuh_midih_get_num_tx_cables(midi_dev_addr) < 1)
        return;

    // transmit any previously queued bytes
    tuh_midi_stream_flush(midi_dev_addr);
    // Blink every interval ms
    if ( board_millis() - start_ms < interval_ms) {
        return; // not enough time
    }
    start_ms += interval_ms;

    uint32_t nwritten = 0;
    // Transmit the note message on the highest cable number
    uint8_t cable = tuh_midih_get_num_tx_cables(midi_dev_addr) - 1;
    nwritten = 0;
    nwritten += tuh_midi_stream_write(midi_dev_addr, cable, message, sizeof(message));
 
    if (nwritten != 0)
    {
        ++message[1];
        ++message[4];
        if (message[1] > last_note)
            message[1] = first_note;
        if (message[4] > last_note)
            message[4] = first_note;
    }
    tuh_midi_stream_flush(midi_dev_addr);
}

// void parse_sysex_message(uint8_t* message, uint16_t length) {
//     // Example parsing based on sysex.txt
//     if (length < 7) {
//         printf("Invalid SysEx message length\n");
//         return;
//     }

//     printf("Parsing SysEx message:\n");
//     printf("Firmware version major: %02X\n", message[3]);
//     printf("Firmware version minor: %02X.%02X\n", message[4], message[5]);
//     printf("Device ID: %02X\n", message[6]);
//     printf("Product ID: %02X\n", message[13]);
//     // Add more parsing as needed based on sysex.txt
// }

// void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets) {
//     uint8_t packet[4];
//     while (tuh_midi_packet_read(dev_addr, packet)) {
//         printf("Received MIDI packet: %02X %02X %02X %02X\n", packet[0], packet[1], packet[2], packet[3]);

//         for (int i = 1; i < 4; i++) {
//             if (packet[i] == 0xF0) {
//                 sysex_in_progress = true;
//                 sysex_index = 0;
//             }

//             if (sysex_in_progress) {
//                 sysex_buffer[sysex_index++] = packet[i];
//                 if (packet[i] == 0xF7) {
//                     sysex_in_progress = false;
//                     parse_sysex_message(sysex_buffer, sysex_index);
//                     sysex_index = 0;
//                 }
//             }
//         }
//     }
// }

int main() {

    bi_decl(bi_program_description("A USB MIDI host example."));

    board_init();
    gpio_init(USB_SWITCH);
    gpio_set_dir(USB_SWITCH, GPIO_IN);

    printf("Pico MIDI Host Example\r\n");
    tusb_init();
    #ifdef RASPBERRYPI_PICO_W
        // for the LED blink
        if (cyw43_arch_init()) {
            printf("WiFi/Bluetooh module init for LED blink failed");
            return -1;
        }

    #endif

 // Set up a repeating timer to trigger every 5 seconds
//reset_usb_bus();
    while (1) {
  
        tuh_task();



        blink_led();
        bool connected = midi_dev_addr != 0 && tuh_midi_configured(midi_dev_addr);
        
        if (connected) {
            send_device_inquiry_request(midi_dev_addr, 0); // Send Device Inquiry Request
            //read the returned bytes back
        }

        //send_next_note(connected);
    }
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_midi_mount_cb(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx)
{
  printf("MIDI device address = %u, IN endpoint %u has %u cables, OUT endpoint %u has %u cables\r\n",
      dev_addr, in_ep & 0xf, num_cables_rx, out_ep & 0xf, num_cables_tx);
          //add_repeating_timer_ms(5000, timer_callback, NULL, &timer);
          //send_device_inquiry_request(midi_dev_addr, 0); // Send Device Inquiry Request

  if (midi_dev_addr == 0) {
    // then no MIDI device is currently connected
    midi_dev_addr = dev_addr;
  }
  

  else {
    printf("A different USB MIDI Device is already connected.\r\nOnly one device at a time is supported in this program\r\nDevice is disabled\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_midi_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  if (dev_addr == midi_dev_addr) {
    midi_dev_addr = 0;
    printf("MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
    }
  else {
    printf("Unused MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  }
}

void tuh_midi_tx_cb(uint8_t dev_addr)
{
    (void)dev_addr;
}
