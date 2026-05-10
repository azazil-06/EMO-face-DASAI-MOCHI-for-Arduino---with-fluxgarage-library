import serial
import time

# Change 'COM3' to your Arduino's port (e.g., '/dev/ttyUSB0' on Linux/Mac)
ser = serial.Serial('COM7', 9600, timeout=1)
time.sleep(2) # Wait for connection to stabilize

print("Listening for Robot events...")

try:
    while True:
        if ser.in_waiting > 0:
            # Read the line sent by Serial.println()
            line = ser.readline().decode('utf-8').strip()
            
            # Split the CSV data
            if "," in line:
                event, value = line.split(",")
                print(f"Robot Event: {event} | Value: {value}")

                # Example Action:
                if event == "DOUBLE_TAP":
                    print(">>> Arduino triggered a heart animation! Sending response...")
                    # You could even send data back to Arduino here:
                    # ser.write(b'THANK_YOU\n')

except KeyboardInterrupt:
    print("Closing connection...")
    ser.close()