import requests
from datetime import datetime, timezone
import json


class Captioner:

	def __init__(self, key: str, filename: str):
		self.url = f"http://upload.youtube.com/closedcaption?cid={key}&seq="
		self.index = 0
		self.captions = []
		with open(filename, 'r') as f:
			for line in f:
				self.captions.append(line)

	def send_next_caption(self):
		if self.index >= len(self.captions):
			print("No more captions to send")
			return

		now = (datetime.now(timezone.utc)).strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3]
		caption = f"{now} region:reg1#cue1\n<br>{self.captions[self.index]}<br>\n"
		r = requests.post(f"{self.url}{self.index}", caption, {'content-type': 'text/plain'})

		self.index += 1
		print(f"Got {r.status_code}: {r.text}")


def main():
	with open("tokens.json", "r") as f:
		token = json.loads(f.read())["youtube"]
	captioner = Captioner(token, "captioner.py")
	captioner.send_next_caption()


if __name__ == "__main__":
	main()
