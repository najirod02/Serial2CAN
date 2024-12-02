#!/bin/bash

# check argument if present
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <usb_connection>"
  exit 1
fi

USB_CONNECTION=$1

# set up the CAN interface
echo "Creating and setting up CAN link for $USB_CONNECTION..."
sudo slcand -o -f -s6 -S115200 "$USB_CONNECTION" can0
if [ $? -ne 0 ]; then
  echo "Failed to create CAN link. Check your connection."
  exit 1
fi

sudo ip link set can0 up type can bitrate 500000
if [ $? -ne 0 ]; then
  echo "Failed to set CAN link up. Check your configuration."
  exit 1
fi

echo "CAN interface setup successfully on can0."
