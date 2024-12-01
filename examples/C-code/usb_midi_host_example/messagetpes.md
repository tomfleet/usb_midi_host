# *Device Enquiry*

*Format of Device Inquiry Request message from Host to Device*

| *byte number* | *Value* | *description*                                       |
| --------------- | --------- | ----------------------------------------------------- |
| *0*            | *0xF0*   | *MIDI System exclusive message start*                |
| *1*            | *0x7E*   | *Non-Real Time Message*                              |
| *2*            | *0x00*   | *Channel to inquire.  (Set to 0 for this protocol.)* |
| *3*            | *0x06*   | *Inquiry Message*                                    |
| *4*            | *0x01*   | *Inquiry Request*                                    |
| *5*            | *0xF7*   | *MIDI System exclusive message terminator*           |

*The ACVx Controller will respond to a Device Inquiry Request message with the following message:*

##### *Format of response from ACVx to Device Inquiry message*

| *byte number* | *Value*                | *description*                                       |
| --------------- | ------------------------ | ----------------------------------------------------- |
| *0*            | *0xF0*                  | *MIDI System exclusive message start*                |
| *1*            | *0x7E*                  | *Non-Real Time Message*                              |
| *2*            | *`<MIDI Channel>`*    | *Common MIDI channel setting*                        |
| *3*            | *0x06*                  | *Inquiry Message*                                    |
| *4*            | *0x02*                  | *Inquiry Response*                                   |
| *5*            | *0x47*                  | *Manufacturers ID Byte*                              |
| *6*            | *`<Product ID>`*      | *Product model ID*                                   |
| *7*            | *0x00*                  | *Number of data bytes to follow (most significant)*  |
| *8*            | *0x19*                  | *Number of data bytes to follow (least significant)* |
| *9*            | *`<Version1>`*        | *Firmware version major most significant*            |
| *10*           | *`<Version2>`*        | *Firmware version major least significant*           |
| *11*           | *`<Version3>`*        | *Firmware version minor most significant*            |
| *12*           | *`<Version4>`*        | *Firmware version minor least significant*           |
| *13*           | *`<DeviceID>`*        | *System Exclusive Device ID*                         |
| *14*           | *`<Serial1>`*         | *Serial Number first digit – unused currently*      |
| *15*           | *`<Serial2>`*         | *Serial Number second digit – unused currently*     |
| *16*           | *`<Serial3>`*         | *Serial Number third digit – unused currently*      |
| *17*           | *`<Serial4>`*         | *Serial Number fourth digit – unused currently*     |
| *18*           | *`<Manufacturing1>`*  | *Manufacturing Data byte 1*                          |
| *19*           | *`<Manufacturing2>`*  | *Manufacturing Data byte 2*                          |
| *20*           | *`<Manufacturing3>`*  | *Manufacturing Data byte 3*                          |
| *21*           | *`<Manufacturing4>`*  | *Manufacturing Data byte 4*                          |
| *22*           | *`<Manufacturing5>`*  | *Manufacturing Data byte 5*                          |
| *23*           | *`<Manufacturing6>`*  | *Manufacturing Data byte 6*                          |
| *24*           | *`<Manufacturing7>`*  | *Manufacturing Data byte 7*                          |
| *25*           | *`<Manufacturing8>`*  | *Manufacturing Data byte 8*                          |
| *26*           | *`<Manufacturing9>`*  | *Manufacturing Data byte 9*                          |
| *27*           | *`<Manufacturing10>`* | *Manufacturing Data byte 10*                         |
| *28*           | *`<Manufacturing11>`* | *Manufacturing Data byte 11*                         |
| *29*           | *`<Manufacturing12>`* | *Manufacturing Data byte 12*                         |
| *30*           | *`<Manufacturing13>`* | *Manufacturing Data byte 13*                         |
| *31*           | *`<Manufacturing14>`* | *Manufacturing Data byte 14*                         |
| *32*           | *`<Manufacturing15>`* | *Manufacturing Data byte 15*                         |
| *33*           | *`<Manufacturing16>`* | *Manufacturing Data byte 16*                         |
| *34*           | *0xF7*                  | *MIDI System exclusive message terminator*           |

# *Outbound Message Type 0x40: Get all button states*

***HARDWARE:** This should be implemented for all new designs.*

*Sending this message to ACVx will cause it to reply with the instantaneous state of all control surface buttons.*

##### *Format of get all button states message (0x40):*

| ***byte number*** | ***value*** | ***description***                             |
| ------------------------- | ------------------- | ----------------------------------------------------- |
| *0*                      | *0xF0*             | *MIDI System exclusive message start*                |
| *1*                      | *0x47*             | *Manufacturers ID*                                   |
| *2*                      | *0x0*              | *Device ID*                                          |
| *3*                      | *`<Product ID>`* | *Product model ID*                                   |
| *4*                      | *0x40*             | *Message type ID*                                    |
| *5*                      | *0x00*             | *Number of data bytes to follow (most significant)*  |
| *6*                      | *0x00*             | *Number of data bytes to follow (least significant)* |
| *7*                      | *0xF7*             | *MIDI System exclusive message terminator*           |

##### *Format of get all button states response message (0x40):*

| ***byte number*** | ***value*** | ***description***                             |
| ------------------------- | ------------------- | ----------------------------------------------------- |
| *0*                      | *0xF0*             | *MIDI System exclusive message start*                |
| *1*                      | *0x47*             | *Manufacturers ID*                                   |
| *2*                      | *0x0*              | *Device ID*                                          |
| *3*                      | *`<Product ID>`* | *Product model ID*                                   |
| *4*                      | *0x40*             | *Message type ID*                                    |
| *5*                      | *0x00*             | *Number of data bytes to follow (most significant)*  |
| *6*                      | *0x13*             | *Number of data bytes to follow (least significant)* |
| *7*                      | *0x00-0x7f*        | *Note States [0x00 … 0x06]*                         |
| *8*                      | *0x00-0x7f*        | *Note States [0x07 … 0x0d]*                         |
| *9*                      | *0x00-0x7f*        | *Note States [0x0e … 0x14]*                         |
| *10*                     | *0x00-0x7f*        | *Note States [0x15 … 0x1b]*                         |
| *11*                     | *0x00-0x7f*        | *Note States [0x1c … 0x22]*                         |
| *12*                     | *0x00-0x7f*        | *Note States [0x23 … 0x29]*                         |
| *13*                     | *0x00-0x7f*        | *Note States [0x2a … 0x30]*                         |
| *14*                     | *0x00-0x7f*        | *Note States [0x31 … 0x37]*                         |
| *15*                     | *0x00-0x7f*        | *Note States [0x38 … 0x3e]*                         |
| *16*                     | *0x00-0x7f*        | *Note States [0x3f … 0x45]*                         |
| *17*                     | *0x00-0x7f*        | *Note States [0x46 … 0x4c]*                         |
| *18*                     | *0x00-0x7f*        | *Note States [0x4d … 0x53]*                         |
| *19*                     | *0x00-0x7f*        | *Note States [0x54 … 0x5a]*                         |
| *20*                     | *0x00-0x7f*        | *Note States [0x5b … 0x61]*                         |
| *21*                     | *0x00-0x7f*        | *Note States [0x62 … 0x68]*                         |
| *22*                     | *0x00-0x7f*        | *Note States [0x69 … 0x6f]*                         |
| *23*                     | *0x00-0x7f*        | *Note States [0x70 … 0x76]*                         |
| *24*                     | *0x00-0x7f*        | *Note States [0x77 … 0x7d]*                         |
| *25*                     | *0x00-0x7f*        | *Note States [0x7e … 0x7f]*                         |
| *26*                     | *0xF7*             | *MIDI System exclusive message terminator*           |

# *inbound - notes*

##### *Midi note-on messages - received*

| *byte number* | *value*          | *description*                                            |
| --------------- | ------------------ | ---------------------------------------------------------- |
| *0*            | *0x9 `<chan>`*  | *MIDI Note-on.*                                           |
| *1*            | *`<ControlID>`* | *identifier for control surface object (“note number”)* |
| *2*            | *data*            | *control value (non-zero, up to 0x7F)*                    |

##### *Midi note-off messages - received*

| *byte number* | *value*          | *description*                                            |
| --------------- | ------------------ | ---------------------------------------------------------- |
| *0*            | *0x8 `<chan>`*  |                                                            |
| *1*            | *`<ControlID>`* | *identifier for control surface object (“note number”)* |
| *2*            | *0x7F*            | *control value (ignored)*                                 |

# *Type CC1: Absolute Controller messages*

| *byte number* | *value*          | *description*                                                                  |
| --------------- | ------------------ | -------------------------------------------------------------------------------- |
| *0*            | *0xB `<chan>`*  | *MIDI Controller. The 4-bit `<chan>` value will be used for the track strips* |
| *1*            | *`<ControlID>`* | *identifier for control surface object*                                         |
| *2*            | *data*            | *control value*                                                                 |
