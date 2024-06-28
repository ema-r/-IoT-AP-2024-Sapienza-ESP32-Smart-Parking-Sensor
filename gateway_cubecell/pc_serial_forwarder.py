import serial
import platform
import os
from cryptography import x509
from cryptography.hazmat.backends import default_backend
import base64
import binascii
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization





def load_ec_key_and_verify_signature(certificate, message, signature):
    try:

        print(f"Message (hex): {binascii.hexlify(message).decode('utf-8')}")
        print(f"Signature (hex): {binascii.hexlify(signature).decode('utf-8')}")
        # Load the certificate
        #certificate = x509.load_pem_x509_certificate(pem_data, default_backend())
        
        # Extract the public key from the certificate
        public_key = certificate.public_key()
        
        pem_public_key = public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
        print(f"Public Key:\n{pem_public_key.decode('utf-8')}")
        
        # Verify the signature
        public_key.verify(
            signature,
            message,
            ec.ECDSA(hashes.SHA256())
        )
        
        return True
    except InvalidSignature:
        return False
    except Exception as e:
        print(f"An error occurred: {e}")
        return False



def load_pem_files(directory):
    pem_files = [f for f in os.listdir(directory) if f.endswith('.pem')]
    certificates = []

    for pem_file in pem_files:
        file_path = os.path.join(directory, pem_file)
        with open(file_path, 'rb') as f:
            pem_data = f.read()
            certificate = x509.load_pem_x509_certificate(pem_data, default_backend())
            cn_value = certificate.subject.get_attributes_for_oid(x509.NameOID.COMMON_NAME)[0].value

            # Print the CN value
            print(f"Common Name (CN): {cn_value}")
            certificates.append({"filename": cn_value, "certificate": certificate})

    return certificates


    
def get_certificate_by_filename(certificates, filename):
    for cert in certificates:
        if cert['filename'] == filename:
            return cert['certificate']
    return None  # Return None if certificate with given filename is not found


def parse_and_decode_string(input_string):
    # Split the input string by ':'
    parts = input_string.split(':')

    if len(parts) >= 4:
        # Extract substrings
        first_substring = parts[0]
        third_substring = parts[2]
        fourth_substring = parts[3]

        # Decode using base64
        try:
            first_decoded = base64.b64decode(first_substring).decode('utf-8')
            third_decoded = base64.b64decode(third_substring)   #kept as binary
            fourth_decoded = base64.b64decode(fourth_substring) #binary data, dig. siganture
            return first_decoded, third_decoded, fourth_decoded
        except Exception as e:
            print(f"Error decoding: {e}")
            return None, None, None
    else:
        print("Input string does not have enough parts separated by ':'")
        return None, None, None




def print_hexadecimal(data):
    if isinstance(data, bytes):
        hex_representation = binascii.hexlify(data).decode('utf-8')
        print(f"Hexadecimal representation: {hex_representation}")
    else:
        print("Input data is not bytes")
    

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
            certs = load_pem_files('./certs')

            while True:
                if ser.in_waiting > 0:
                    data = ser.readline().decode('utf-8').strip()
                    print(f"Received: {data}")

                    name, message, signature = parse_and_decode_string(data)
                    print_hexadecimal(signature)
                    
                    certificate = get_certificate_by_filename(certs, name)
                    is_valid = load_ec_key_and_verify_signature(certificate, message, signature)
                    print(f"Signature is valid: {is_valid}")

                    
    except serial.SerialException as e:
        print(f"Error opening {port_name}: {e}")

if __name__ == "__main__":
    main()
