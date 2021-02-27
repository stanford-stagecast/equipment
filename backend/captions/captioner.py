import requests
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, HTTPServer
import sys
import socket
import json


class Captioner:
	"""
	This class captions YouTube livestreams.
	"""

	def __init__(self, token_file: str, filename: str):
		"""
		The token file is the serialization of a JSON object with two keys, "youtube" and "last_seqno"
		The key "youtube" maps to the stream key for the relevant livestream.
		The key "last_seqno" maps to the sequence number of the last caption sent to YouTube. It is updated
		when this program exits
		:param token_file: A path to the file with the tokens
		:param filename: A path to a file with each caption on a separate line
		"""
		try:
			with open(token_file, 'r') as f:
				tokens = json.loads(f.read())
		except FileNotFoundError:
			print(f"No such file or directory {token_file}")
			raise FileNotFoundError

		self.token_file = token_file
		self.key = tokens["youtube"]
		self._url = f"http://upload.youtube.com/closedcaption?cid={self.key}&seq="
		self._seqno = tokens["last_seqno"] + 1
		self.index = 0
		self._captions = []
		self._first_caption = "No previous captions"
		self._last_caption = "No more captions"

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
		if self.index >= len(self._captions):
			print("No more captions to send")
			return

		now = (datetime.now(timezone.utc)).strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3]
		caption = f"{now} region:reg1#cue1\n<br>{self.get_caption(self.index)}<br>\n".encode("utf-8")
		r = requests.post(f"{self._url}{self.index + self._seqno}", caption, {'content-type': 'text/plain; charset=UTF-8'})

		self.index += 1
		self._seqno += 1
		print(f"Captioner got <{r.status_code}>: {r.text}")

	def get_caption(self, i):
		"""
		:return: A string containing the next caption to be sent on
		"""
		if i < 0:
			return self._first_caption
		elif i >= len(self._captions):
			return self._last_caption
		else:
			return self._captions[i]

	def next(self):
		self.index = min(self.index + 1, len(self._captions) - 1)

	def back(self):
		self.index = max(self.index - 1, 0)

	def reset(self):
		self.index = 0

	def save_tokens(self):
		with open(self.token_file, 'w') as f:
			tokens = {"youtube": self.key, "last_seqno": self._seqno}
			f.write(json.dumps(tokens))


class CaptionHandler(BaseHTTPRequestHandler):
	def send_page(self):
		"""
		Constructs an HTML document containing the next caption to send and posts it
		:return: Nothing
		"""
		self.protocol_version = 'HTTP/1.1'
		prev_caption = self.server.captioner.get_caption(self.server.captioner.index - 1)
		curr_caption = self.server.captioner.get_caption(self.server.captioner.index)
		next_caption = self.server.captioner.get_caption(self.server.captioner.index + 1)
		document = bytes(self.server.html.format(prev_caption, curr_caption, next_caption), "utf-8")

		self.send_response(200)
		self.send_header("Content-type", "text/html; charset=UTF-8")
		self.send_header("Content-Length", str(len(document)))
		self.end_headers()
		self.wfile.write(document)

	def do_GET(self):
		self.send_page()

	def do_POST(self):
		"""
		Sends the next caption and updates the webpage
		:return:
		"""
		length = int(self.headers.get("Content-Length"))
		body = str(self.rfile.read(length))
		response = body[body.find('=')+1:-1]

		if response == "send":
			print("Sending a caption")
			self.server.captioner.send_next_caption()
		elif response == "next":
			print("Staging next caption")
			self.server.captioner.next()
		elif response == "back":
			print("Staging previous caption")
			self.server.captioner.back()
		elif response == "reset":
			print("Resetting caption counter")
			self.server.captioner.reset()

		self.send_page()


class CaptionServer(HTTPServer):
	def __init__(self, address, html_file: str, token_file: str, caption_file: str):
		"""
		:param address: A pair containing a hostname and port where the server will listen
		:param html_file: The path to a skeleton HTML file to be populated with information
		:param token_file: See Captioner class
		:param caption_file: The path to the file with the captions
		"""
		super().__init__(address, CaptionHandler)
		self.captioner = Captioner(token_file, caption_file)
		try:
			with open(html_file, 'r') as f:
				self.html = f.read()
		except FileNotFoundError:
			print(f"No such file or directory {html_file}")


def main():
	# Parse args
	if len(sys.argv) == 1:
		print("No arguments given. Running with defaults. Try 'captioner --help' for more information")
		hostname = "0.0.0.0"
		port = 9000
		html = "index.html"
		token_file = "tokens.json"
		caption_file = "captions.txt"
	elif sys.argv[1] == "--help" or len(sys.argv) != 6:
		print("Usage: captioner <hostname> <port> <html file> <token file> <caption file>")
		return 0
	else:
		hostname = sys.argv[1]
		port = int(sys.argv[2])
		html = sys.argv[3]
		token_file = sys.argv[4]
		caption_file = sys.argv[5]

	try:
		server = CaptionServer((hostname, port), html, token_file, caption_file)
		print(f"Starting CaptionServer at {hostname}:{port} with html file {html}, stream key {server.captioner.key}, and caption file {caption_file}")
	except FileNotFoundError:
		print("Failed to construct CaptionServer")
		return 1
	except socket.gaierror:
		print(f"Name or service unknown: {hostname}:{port}")
		return 1

	try:
		server.serve_forever()
	except KeyboardInterrupt:
		print("Saving tokens")
		server.captioner.save_tokens()

	print("Closing CaptionServer")
	server.server_close()
	return 0


if __name__ == "__main__":
	main()
