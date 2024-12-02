# USB2CAN

Serializer to read and write data from can bus using an usb connection.

Table of Content:
- [General info](#general-info)
- [Setup the can interface on Linux](#setup-the-can-interface-on-linux)
- [CAN mask configuration on STM32](#can-mask-configuration-on-stm32)
- [Slcand encapsulation format](#slcand-encapsulation-format)
- [Send CAN frames](#send-can-frames)
- [Dump CAN frames](#dump-can-frames)


## General info

Board: Nucleo F446RE

For this project, the can is set in loopback mode in order to conduct tests more easily.<br/>
For a "real" test it is possible to connect two board and set the can interface in normal mode.

`Important notes:`<br/>

__Loopback mode__: set PA11 to Pull-up

__Normal mode__: set PA11 to No pull-up and no pull-down


## Setup the can interface on Linux
In order to send CAN frames, we need first to setup the interface by using slcand, a module of the can-utils utility of Linux.<br/>
After that, we can use cansend and candump to send and receive frames.<br/>
Slcand allows us to generate a frame, send it through the UART connection. Before forwarding it to the "real" can bus, the board must create a can frame.

```bash
sudo apt-get install can-utils

sudo slcand -o -s6 -S115200 /dev/ttyACM0 can0

sudo ip link set can0 up type can bitrate 500000

ip link show can0
```

This will create a can interface with baud rate of 50K.

## CAN mask configuration on STM32
It is possible not only to set a mask directly on the terminal of the host pc but also 
on the board to accept or reject the frames it receives.<br/>
The file __can.c__ contains the function __MX_CAN1_Init__ in which it is possible to define our mask.<br/>
The mask is set to 'anything' so, every frame the board receives will be taken and forwarded to the UART.


## Slcand encapsulation format
Slcand uses a specific encapsulation format to send and receive the can frames, more details can be found [here](https://github.com/torvalds/linux/blob/master/drivers/net/can/slcan/slcan-core.c).

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

```bash
cangen can0
```
If we want to generate a series of random frames.


## Dump CAN frames
Simply run the following command:
```bash
candump can0
```
All traffic will be intercepted (based on the mask of the board/terminal) and will be
displayed on terminal.


## Authors
Dorijan Di Zepp dorijan.dizepp@eagletrt.it