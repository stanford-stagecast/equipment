import sys
import json

import tornado.ioloop
import tornado.web

from client import ClientHandler
from export import ExportHandler

from state import State
from serializer import deserialize


class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        self.redirect("/index.html")


def make_app(static_path: str) -> tornado.web.Application:
    return tornado.web.Application(
        [
            (r"/socket", ClientHandler),
            (r"/export(.json)?", ExportHandler),
            (r"/", IndexHandler),
            (r"/(.*)", tornado.web.StaticFileHandler, {"path": static_path}),
        ]
    )


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python main.py <port> <static file path>")
        exit(1)

    try:
        port = int(sys.argv[1])
        print("Listening on port", port)
    except ValueError:
        print("Could not understand port.")
    path = sys.argv[2]
    save = "lights.json"
    try:
        with open(save, "r") as f:
            text = f.read()
            data = json.loads(text)
            lst = deserialize(data)
            if lst is None:
                print("Save file corrupted.")
            else:
                State.set(lst)
    except FileNotFoundError:
        print("Unable to find save file:", save)
    except json.JSONDecodeError:
        print("Unable to understand save file:", save)
    print("Using save file", save)
    print("Using static files from", path)

    app = make_app(path)
    app.listen(port)
    tornado.ioloop.IOLoop.current().start()
