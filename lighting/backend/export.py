import json
import tornado.web
from serializer import serialize
from state import State


class ExportHandler(tornado.web.RequestHandler):
    def get(self, _):
        cues = State.get()

        self.set_header('Content-Type', 'text/json; charset="utf-8"')
        self.write(json.dumps(serialize(cues)))
