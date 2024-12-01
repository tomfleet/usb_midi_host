#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#define SYSEX_BUFFER_SIZE 256

typedef struct {
    uint8_t midi_channel;
    uint8_t manufacturer_id;
    uint8_t product_id;
    uint8_t firmware_version_major;
    uint8_t firmware_version_minor_major;
    uint8_t firmware_version_minor_minor;
    uint8_t device_id;
    // Add more fields as needed based on messagetypes.md
} MidiMessageData;

extern MidiMessageData midi_message_data;

void parse_sysex_message(uint8_t* message, uint16_t length);
void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets);
void send_device_inquiry_request(uint8_t dev_addr, uint8_t cable_num);
void send_get_all_button_states_message(uint8_t dev_addr, uint8_t cable_num, uint8_t product_id);

#endif // MIDI_HANDLER_H