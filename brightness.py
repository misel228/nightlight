import serial
import re
import argparse

def map_brightness_to_percent(value):
    return int((value / 1024) * 100)

def read_serial(port, baudrate, output_file):
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print(f"Listening on {port} at {baudrate} baud...")
            pattern = re.compile(r"Brightness:\s*(\d+)")

            while True:
                line = ser.readline().decode('utf-8').strip()
                match = pattern.search(line)

                if match:
                    brightness_value = int(match.group(1))
                    if 0 <= brightness_value <= 1023:
                        percent = map_brightness_to_percent(brightness_value)
                        if percent > 70:
                            display_brightness = 255;
                        elif percent > 10:
                            display_brightness = 150;
                        else:
                            display_brightness = 25;
                        print(f"Brightness: {brightness_value} -> {percent}% -> {display_brightness}")
                        with open(output_file, 'w') as f:
                            f.write(f"{display_brightness}\n")
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("Stopped by user.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Read brightness from serial port and write as percentage to file.")
    parser.add_argument("--port", required=True, help="Serial port (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("--baudrate", type=int, default=9600, help="Baud rate (default: 9600)")
    parser.add_argument("--output", default="brightness.txt", help="Output file (default: brightness.txt)")

    args = parser.parse_args()
    read_serial(args.port, args.baudrate, args.output)
