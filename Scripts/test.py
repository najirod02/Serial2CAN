import can

def send_can_frame(interface, can_id, data):
    """
    Sends a single CAN frame on the specified interface.

    Args:
        interface (str): The CAN interface name (e.g., 'vcan0').
        can_id (int): The CAN ID for the frame.
        data (list): A list of data bytes to include in the frame (max 8 bytes).
    """
    # Create a CAN bus instance for the specified interface
    with can.Bus(channel=interface, interface='socketcan') as bus:
        # Create a CAN message
        msg = can.Message(arbitration_id=can_id, data=data, is_extended_id=False)

        try:
            # Send the CAN message
            bus.send(msg)
            print(f"Message sent on {interface}: ID={hex(can_id)}, Data={data}")
        except can.CanError as e:
            print(f"Failed to send message: {e}")

def receive_can_frame(interface, timeout=5):
    """
    Waits to receive a single CAN frame on the specified interface.

    Args:
        interface (str): The CAN interface name (e.g., 'vcan0').
        timeout (int): Timeout in seconds to wait for a frame (default: 5).

    Returns:
        None
    """
    # Create a CAN bus instance for the specified interface
    with can.Bus(channel=interface, interface='socketcan') as bus:
        print(f"Waiting for a message on {interface} (timeout: {timeout}s)...")
        try:
            # Wait for a CAN message
            msg = bus.recv(timeout=timeout)
            if msg:
                print("Message received!")
                print(f"ID: {hex(msg.arbitration_id)}")
                print(f"Data: {list(msg.data)}")
                print(f"DLC: {msg.dlc}")
                print(f"Is Extended ID: {msg.is_extended_id}")
                print(f"Is Remote Frame: {msg.is_remote_frame}")
                print(f"Timestamp: {msg.timestamp}")
            else:
                print(f"No message received within {timeout} seconds.")
        except Exception as e:
            print(f"Error receiving message: {e}")

if __name__ == "__main__":
    # Virtual CAN interface name
    interface_name = "vcan0"

    # Example CAN frame parameters
    example_can_id = 0x123  # Standard 11-bit CAN ID
    example_data = [0xDE, 0xAD, 0xBE, 0xEF]  # Example payload (4 bytes)

    # Send the frame
    send_can_frame(interface_name, example_can_id, example_data)

    # Wait to receive a frame
    receive_can_frame(interface_name)
