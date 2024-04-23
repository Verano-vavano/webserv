#!/usr/bin/python3.10 
import sys
import requests

def upload_file(file_path, uri='/uploaded', ip='127.0.0.1', port=8080):
	url = f'http://{ip}:{port}{uri}'
	try:
		with open(file_path, 'rb') as file:
			response = requests.post(url, data=file)
		print(response.raw.read().decode('utf8'))
	except requests.exceptions.ConnectionError as e:
		print(f"Connection error: {e}")
	except requests.exceptions.RequestException as e:
		print(f"Error: {e}")

if __name__ == "__main__":
	if len(sys.argv) > 1:
		file_path = sys.argv[1]
	else:
		print("Please provide a file path.")
		sys.exit(1)
	
	uri = '/uploaded/' + sys.argv[1] if len(sys.argv) < 3 else sys.argv[2]
	ip = '127.0.0.1' if len(sys.argv) < 4 else sys.argv[3]
	port = 8080 if len(sys.argv) < 5 else int(sys.argv[4])

	upload_file(file_path, uri, ip, port)
