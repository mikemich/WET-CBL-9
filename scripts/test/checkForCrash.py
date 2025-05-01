import os
import sys
import serial
from serial.tools import list_ports
import time

serialString = ""
ser = None


def find_stm_port():
    ports = list_ports.comports()
    for port in ports:
        if "STM" in port.manufacturer:
            return port.device
    return None


if __name__ == "__main__":
    # Automatically detect the COM port
    port = find_stm_port()
    if not port:
        sys.exit("Error: Could not find STM COM port")

    try:
        ser = serial.Serial(port, 115200)
    except IOError:
        sys.exit("Error: Could not open serial port " + port)

    # Timeout in seconds
    timeout = 10
    start_time = time.time()

    # Wait for a response within the timeout period
    while time.time() - start_time < timeout:
        if ser.in_waiting > 0:
            serialString += ser.read().decode()

    print(serialString)

    if "***CRASH***" in serialString:
        print("Crash detected!")
        ser.close()
        sys.exit(1)

    print("No crash seems to be detected...")
    ser.close()
    sys.exit(0)
