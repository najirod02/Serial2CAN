#!/bin/bash

# check argument if present
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <canif name>" 
  exit 1
fi

CAN_NAME=$1

# kill the slcand process associated with canX
echo "Detaching slcand..."

# check if canX exists
if ! ip link show "$CAN_NAME" > /dev/null 2>&1; then
  echo "CAN interface "$CAN_NAME" is not active."
  exit 1
fi

# set down canX
echo "Bringing down CAN interface "$CAN_NAME"..."
sudo ip link set "$CAN_NAME" down
if [ $? -ne 0 ]; then
  echo "Failed to bring down "$CAN_NAME"."
  exit 1
fi

sudo pkill -f "slcand.*"$CAN_NAME""
if [ $? -ne 0 ]; then
  echo "Failed to detach slcand or no slcand process found."
  exit 1
fi

echo "CAN interface and slcand successfully detached."