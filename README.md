# USB2CAN

Serializer to read and write data from can bus using an usb connectio.

Board: Nucleo F446RE

From the official manual the features supported on ST-LINK/V2-1 for the USB are:
- USB software re-enumeration
- Virtual COM port interface on USB
- Mass storage interface on USB
- USB power management request for more than 100 mA power on USB


## Setup the can interface
In order to send CAN frames, we need first to setup the interface. After that,
we can use cansend and candumb to send and receive frames from the can bus.
Note that the frames we are going to send will be "converted" into a serial message
so the board must read it and create a complete and correct can frame.

```bash
sudo slcand -o -c /dev/ttyUSB0 can0

sudo ip link set can0 up type can bitrate 500000

ip link show can0
```

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

## CAN
At the moment, tested sending and receving on looppback interface
