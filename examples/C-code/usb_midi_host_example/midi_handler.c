#include "midi_handler.h"
#include "tusb.h"
#include <string.h>
#include <stdio.h>

MidiMessageData midi_message_data;

uint8_t sysex_buffer[SYSEX_BUFFER_SIZE];
uint16_t sysex_index = 0;
bool sysex_in_progress = false;

void parse_sysex_message(uint8_t* message, uint16_t length) {
    // Example parsing based on messagetypes.md
    if (length < 17) {
        printf("Invalid SysEx message length\n");
        return;
    }

    printf("Parsing SysEx message, %d bytes:\n", length);  
    for(int i = 0; i < length; i++) {
        printf("%02X ", message[i]);
    }
    printf("\n");
    midi_message_data.midi_channel = message[2];
    midi_message_data.manufacturer_id = message[5];
    midi_message_data.product_id = message[6];
    midi_message_data.firmware_version_major = message[9]<< 4 | message[10];
    midi_message_data.firmware_version_minor_major = message[10]<< 4 | message[11];
    midi_message_data.firmware_version_minor_minor = 0;//message[11];
    midi_message_data.device_id = message[12];

    printf("MIDI Channel: %02X\n", midi_message_data.midi_channel);
    printf("Manufacturer ID: %02X\n", midi_message_data.manufacturer_id);
    printf("Product ID: %02X\n", midi_message_data.product_id);
    printf("Firmware version major: %02X\n", midi_message_data.firmware_version_major);
    printf("Firmware version minor: %02X.%02X\n", midi_message_data.firmware_version_minor_major, midi_message_data.firmware_version_minor_minor);
    printf("Device ID: %02X\n", midi_message_data.device_id);
    // Add more parsing as needed based on messagetypes.md
}

void handle_note_on(uint8_t channel, uint8_t note, uint8_t velocity) {
    printf("Note On: Channel=%d, Note=%d, Velocity=%d\n", channel, note, velocity);
}

void handle_note_off(uint8_t channel, uint8_t note, uint8_t velocity) {
    printf("Note Off: Channel=%d, Note=%d, Velocity=%d\n", channel, note, velocity);
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets) {
    
   // printf("MIDI RX Callback: Device address = %u, Number of packets = %u\n", dev_addr, num_packets);

    uint8_t packet[4];
    while (tuh_midi_packet_read(dev_addr, packet)) {
        printf("Received MIDI packet: %02X %02X %02X %02X\n", packet[0], packet[1], packet[2], packet[3]);

        uint8_t cable_num = packet[0] >> 4;
        uint8_t code_index_number = packet[0] & 0x0F;

        if (code_index_number == 0x09) { // Note On
            handle_note_on(packet[1] & 0x0F, packet[2], packet[3]);
        } else if (code_index_number == 0x08) { // Note Off
            handle_note_off(packet[1] & 0x0F, packet[2], packet[3]);
        }

        for (int i = 0; i < 4; i++) {
            if (packet[i] == 0xF0) {
                sysex_in_progress = true;
                sysex_index = i-1;
            }

            if (sysex_in_progress) {
                sysex_buffer[sysex_index++] = packet[i];
                if (sysex_index >= SYSEX_BUFFER_SIZE) {
                    printf("SysEx buffer overflow\n");
                    sysex_in_progress = false;
                    sysex_index = 0;
                    break;
                }
                if (packet[i] == 0xF7) {
                    sysex_in_progress = false;
                    parse_sysex_message(sysex_buffer, sysex_index);
                    sysex_index = 0;
                }
            }
        }
    }
}

void send_device_inquiry_request(uint8_t dev_addr, uint8_t cable_num) {
    uint8_t message[] = {
        0xF0, 0x7E, 0x00, 0x06, 0x01, 0xF7
    };
    tuh_midi_stream_write(dev_addr, cable_num, message, sizeof(message));
    tuh_midi_stream_flush(dev_addr);
}