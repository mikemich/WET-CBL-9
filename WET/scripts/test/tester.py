import os
import subprocess
import sys
import serial
import time
from serial.tools import list_ports
import argparse

serialString = ""
isResultPass = False
ser = None


def find_stm_port():
    ports = list_ports.comports()
    for port in ports:
        if "STM" in port.manufacturer:
            return port.device
    return None


def optionalChecks(string):
    print(f"Optinal check: {string} test is running...")
    result = subprocess.run(
        ["python", os.path.join(f"./{string}.py")], capture_output=True, text=True
    )
    print("External script output:")
    print(result.stdout)
    if result.returncode == 0:
        print(f"{string} test passed")
    else:
        print(f"{string} test failed with exit code", result.returncode)

    return result.returncode


# main
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test script for FlightComputer")
    parser.add_argument("test_id", type=int, help="Test ID to run")
    parser.add_argument(
        "--optional",
        type=str,
        help="Optional argument for additional checks",
        default=None,
    )
    args = parser.parse_args()

    # Automatically detect the COM port
    port = find_stm_port()
    if not port:
        sys.exit("Error: Could not find STM COM port")

    try:
        ser = serial.Serial(port, 115200)
    except IOError:
        sys.exit("Error: Could not open serial port " + port)

    # Format the test ID to 3 digits with leading zeros
    formatted_arg = f"{args.test_id:03}"
    print("Running test: ", formatted_arg)

    # Send the test ID to the serial port with leading zeros
    ser.write(formatted_arg.encode())

    # Timeout in seconds
    timeout = 10
    start_time = time.time()

    # Wait for a response within the timeout period
    while time.time() - start_time < timeout:
        if ser.in_waiting > 0:
            serialString += ser.read().decode()
            break

    if serialString == "":
        ser.close()
        sys.exit("Timed out with no reply...")

    # There is a connection, wait for the test to finish
    while 1:
        if ser.in_waiting > 0:
            char = ser.read(ser.in_waiting)
            serialString += char.decode()

            # check regex for ***PASS or ***FAIL
            if "\033[42m***OK***\033[0m" in serialString:
                print("Test Passed...")
                isResultPass = True
                break
            elif "\033[41m***FAIL***\033[0m" in serialString:
                print("Test Failed...")
                isResultPass = False
                break

    # Handle the optional argument
    if args.optional:
        result = optionalChecks(args.optional)
        if result != 0:  # if the optional check fails, the test is considered failed
            isResultPass = False

    print(serialString)

    ser.close()

    if isResultPass:
        sys.exit(0)
    else:
        sys.exit("Test failed")
