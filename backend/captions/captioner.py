import requests
from datetime import datetime, timezone, timedelta
from time import sleep

def make_one_caption(n, t):
	now = (datetime.now(timezone.utc) + timedelta(seconds=t)).strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3]
	return "{} region:reg1#cue1\n<br>Caption {}<br>\n".format(now, n)

def main():
	url = "http://upload.youtube.com/closedcaption?cid=vp4u-e3zq-2xzu-1wgs-azkd"
	for i in range(10):
		caption = make_one_caption("{}a".format(i), 0) + make_one_caption("{}b:".format(i), 1)
		r = requests.post("{}&seq={}".format(url, 2*i+22520), caption, {'content-type': 'text/plain'})
		print("Sent {}\nGot {}: {}".format(caption, r.status_code, r.text))
		sleep(5)


if __name__ == "__main__":
	main()
