#!/usr/bin/python3.10 

import socket
import sys

def upload_file(filename, ip='localhost', port=8080):
	try:
		with open(filename, 'rb') as file:
			file_data = file.read()

		# Create a TCP/IP socket
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
			# Connect to the server
			s.connect((ip, port))
			print(f"Connected to {ip}:{port}")

			# Construct the HTTP POST request
			http_request = (
				f"POST /uploaded/" + filename + " HTTP/1.1\r\n"
				f"Host: {ip}:{port}\r\n"
				f"Content-Type: application/octet-stream\r\n"
				f"Content-Disposition: form-data; name=\"file\"; filename=\"{filename}\"\r\n"
				f"Content-Length: {len(file_data)}\r\n\r\n"
			).encode() + file_data

			# Send the HTTP POST request
			s.sendall(http_request)

			print(f"File '{filename}' uploaded successfully.")
	except FileNotFoundError:
		print(f"Error: File '{filename}' not found.")
	except Exception as e:
		print("Error:", e)

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("Usage: python upload_file.py <filename> [ip] [port]")
		sys.exit(1)

	filename = sys.argv[1]
	ip = sys.argv[2] if len(sys.argv) >= 3 else 'localhost'
	port = int(sys.argv[3]) if len(sys.argv) == 4 else 8080

	upload_file(filename, ip, port)
