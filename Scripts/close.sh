#!/bin/bash

# kill the slcand process associated with can0
echo "Detaching slcand..."

# check if can0 exists
if ! ip link show can0 > /dev/null 2>&1; then
  echo "CAN interface can0 is not active."
  exit 1
fi

# set down can0
echo "Bringing down CAN interface can0..."
sudo ip link set can0 down
if [ $? -ne 0 ]; then
  echo "Failed to bring down can0."
  exit 1
fi

sudo pkill -f "slcand.*can0"
if [ $? -ne 0 ]; then
  echo "Failed to detach slcand or no slcand process found."
  exit 1
fi

echo "CAN interface and slcand successfully detached."