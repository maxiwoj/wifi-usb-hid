#!/bin/bash
# Script to generate self-signed certificate for ESP8266 HTTPS server

echo "Generating self-signed certificate for ESP8266..."

# Generate private key and certificate
openssl req -x509 -nodes -newkey rsa:2048 -days 3650 \
  -keyout server_key.pem -out server_cert.pem \
  -subj "/C=US/ST=State/L=City/O=WiFi-USB-HID/CN=192.168.4.1"

echo ""
echo "Converting to DER format..."
openssl rsa -in server_key.pem -outform DER -out server_key.der
openssl x509 -in server_cert.pem -outform DER -out server_cert.der

echo ""
echo "Converting to C array format..."

# Convert key to hex array
echo "const uint8_t server_key[] PROGMEM = {" > certs.h
xxd -i < server_key.der | sed 's/^/  /' >> certs.h
echo "};" >> certs.h
echo "const size_t server_key_len = sizeof(server_key);" >> certs.h
echo "" >> certs.h

# Convert certificate to hex array
echo "const uint8_t server_cert[] PROGMEM = {" >> certs.h
xxd -i < server_cert.der | sed 's/^/  /' >> certs.h
echo "};" >> certs.h
echo "const size_t server_cert_len = sizeof(server_cert);" >> certs.h

echo ""
echo "Certificate files generated:"
echo "  - server_key.pem (private key)"
echo "  - server_cert.pem (certificate)"
echo "  - server_key.der (key in DER format)"
echo "  - server_cert.der (cert in DER format)"
echo "  - certs.h (C header for ESP8266)"
echo ""
echo "The certificate is valid for 10 years and is self-signed."
echo "Browsers will show a security warning - this is expected for self-signed certificates."
