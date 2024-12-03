#!/bin/bash

# check argument if present
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <usb_connection> <canif name>" 
  exit 1
fi

USB_CONNECTION=$1
CAN_NAME=$2

# set up the CAN interface
echo "Creating and setting up CAN link for $USB_CONNECTION..."
sudo slcand -o -c -f -s6 "$USB_CONNECTION" "$CAN_NAME"
if [ $? -ne 0 ]; then
  echo "Failed to create CAN link. Check your connection."
  exit 1
fi

sudo ip link set "$CAN_NAME" up type can bitrate 500000
if [ $? -ne 0 ]; then
  echo "Failed to set CAN link up. Check your configuration."
  exit 1
fi

echo "CAN interface setup successfully on can0."
