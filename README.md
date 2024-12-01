# USB2CAN

Serializer to read and write data from can bus using an usb connection.

Board: Nucleo F446RE

__Loopback mode__: set PA11 to Pull-up

From the official manual the features supported on ST-LINK/V2-1 for the USB are:
- USB software re-enumeration
- Virtual COM port interface on USB
- Mass storage interface on USB
- USB power management request for more than 100 mA power on USB

For this project, the can is set in loopback mode in order to conduct test more easily.<br/>
For a "real" test it is possible to connect two board and set the can interface in normal mode.

## Setup the can interface
In order to send CAN frames, we need first to setup the interface by using slcand, a module of the can-utils utility of Linux.<br/>
After that, we can use cansend and candump to send and receive frames.<br/>
Slcand allows us to generate a frame, send it through the UART connestion, before forwarding it to the "real" can bus, the board must fetch all the information from the UART and after that, can create a frame.

```bash
sudo apt-get install can-utils

sudo slcand -o -c -s6 -S115200 /dev/ttyACM0 can0

sudo ip link set can0 up type can bitrate 500000

ip link show can0
```

This will create a can interface with baud rate of 50K.

## Slcand encapsulation format.
Slcand uses a speciifc encapsulation format to send and receive the can frames, more details can be found [here](https://github.com/torvalds/linux/blob/master/drivers/net/can/slcan/slcan-core.c).

Briefly, we have:
- type: t, r, T, R
- id: 3 (standard) or 8 (extended) bytes in ASCII Hex (base64)
- dlc: one byte ASCII number ('0' - '8')
- data: section has at much ASCII Hex bytes as defined by the dlc

## Send CAN frames
Simply run the following command:
```bash
cansend can0 123#DEADBEEF
```
where as example we send with ID = 123 the payload DEADBEEF.

## Read CAN frames
Simply run the following command:
```bash
candump can0
```
All traffic will be intercepted (based on the mask of the board) and will be
showed on terminal.