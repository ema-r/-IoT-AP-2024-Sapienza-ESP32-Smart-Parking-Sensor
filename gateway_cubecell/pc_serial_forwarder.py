import serial
import platform

def main():
    system_type = input("Is your system Unix or Windows? (Enter 'unix' or 'windows'): ").strip().lower()
    
    if system_type == 'windows':
        port_number = int(input("Enter the COM port number: "))
        port_name = f"COM{port_number}"
    elif system_type == 'unix':
        port_name = input("Enter the tty file path (e.g., /dev/ttyUSB0): ").strip()
    else:
        print("Invalid system type entered. Please run the script again.")
        return

    # Try to open the serial port
    try:
        with serial.Serial(port_name, baudrate=115200, timeout=1) as ser:
            print(f"Opened {port_name} successfully. Reading data...")
            while True:
                if ser.in_waiting > 0:
                    data = ser.readline().decode('utf-8').strip()
                    print(f"Received: {data}")
    except serial.SerialException as e:
        print(f"Error opening {port_name}: {e}")

if __name__ == "__main__":
    main()
