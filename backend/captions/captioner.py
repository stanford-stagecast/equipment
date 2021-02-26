import requests
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, HTTPServer
import sys
import socket


class Captioner:
	"""
	This class captions YouTube livestreams.
	"""

	def __init__(self, key: str, filename: str):
		"""
		:param key: The YouTube stream key as a string
		:param filename: A path to a file with each caption on a separate line
		"""
		self._url = f"http://upload.youtube.com/closedcaption?cid={key}&seq="
		self._index = 0
		self._captions = []
		try:
			with open(filename, 'r') as f:
				for line in f:
					self._captions.append(line.strip())
		except FileNotFoundError:
			print(f"No such file or directory {filename}")
			raise FileNotFoundError

	def send_next_caption(self):
		"""
		This method sends the next caption to YouTube
		:return: nothing
		"""
		if self._index >= len(self._captions):
			print("No more captions to send")
			return

		now = (datetime.now(timezone.utc)).strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3]
		caption = f"{now} region:reg1#cue1\n<br>{self._captions[self._index]}<br>\n"
		r = requests.post(f"{self._url}{self._index}", caption, {'content-type': 'text/plain'})

		self._index += 1
		print(f"Captioner got <{r.status_code}>: {r.text}")

	def get_next_caption(self):
		"""
		:return: A string containing the next caption to be sent on
		"""
		if self._index >= len(self._captions):
			return "No more captions"
		else:
			return self._captions[self._index]


class CaptionHandler(BaseHTTPRequestHandler):
	def send_page(self):
		"""
		Constructs an HTML document containing the next caption to send and posts it
		:return: Nothing
		"""
		document = self.server.html.format(self.server.captioner.get_next_caption())
		self.send_response(200)
		self.send_header("Content-type", "text/html")
		self.end_headers()
		self.wfile.write(bytes(document, "utf-8"))

	def do_GET(self):
		self.send_page()

	def do_POST(self):
		"""
		Sends the next caption and updates the webpage
		:return:
		"""
		self.server.captioner.send_next_caption()
		self.send_page()


class CaptionServer(HTTPServer):
	def __init__(self, address, html_file: str, key: str, caption_file: str):
		"""
		:param address: A pair containing a hostname and port where the server will listen
		:param html_file: The path to a skeleton HTML file to be populated with information
		:param key: The YouTube stream key
		:param caption_file: The path to the file with the captions
		"""
		super().__init__(address, CaptionHandler)
		self.captioner = Captioner(key, caption_file)
		try:
			with open(html_file, 'r') as f:
				self.html = f.read()
		except FileNotFoundError:
			print(f"No such file or directory {html_file}")


def main():
	# Parse args
	if len(sys.argv) == 1:
		print("No arguments given. Running with defaults. Try 'captioner --help' for more information")
		hostname = "localhost"
		port = 8000
		html = "index.html"
		key_file = "key.txt"
		caption_file = "captions.txt"
	elif sys.argv[1] == "--help" or len(sys.argv) != 6:
		print("Usage: captioner <hostname> <port> <html file> <stream key file> <caption file>")
		return 0
	else:
		hostname = sys.argv[1]
		port = int(sys.argv[2])
		html = sys.argv[3]
		key_file = sys.argv[4]
		caption_file = sys.argv[5]

	try:
		with open(key_file, "r") as f:
			key = f.read().strip()
	except FileNotFoundError:
		print(f"No such file or directory {key_file}")
		return

	try:
		server = CaptionServer((hostname, port), html, key, caption_file)
		print(f"Starting CaptionServer at {hostname}:{port} with html file {html}, stream key {key}, and caption file {caption_file}")
	except FileNotFoundError:
		print("Failed to construct CaptionServer")
		return 1
	except socket.gaierror:
		print(f"Name or service unknown: {hostname}:{port}")
		return 1

	try:
		server.serve_forever()
	except KeyboardInterrupt:
		pass

	print("Closing CaptionServer")
	server.server_close()
	return 0


if __name__ == "__main__":
	main()
