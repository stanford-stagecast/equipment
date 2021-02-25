import json
import tornado.websocket
from cue_list import CueList
from serializer import serialize, deserialize
from state import State


class ClientHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        self.sync()

    def on_message(self, message):
        try:
            data = json.loads(message)
        except json.JSONDecodeError:
            return self.respond_with_error("invalid JSON")
        if "type" not in data:
            return self.respond_with_error("no message type")
        tp = data["type"]
        if tp == "sync":
            return self.sync()
        elif tp == "update":
            return self.update(data)
        elif tp == "save":
            return self.save()
        return self.respond_with_error("invalid request")

    def sync(self):
        cues = State.get()
        self.send({"type": "sync", "data": serialize(cues)})

    def update(self, data: dict):
        if "data" not in data:
            return self.respond_with_error("no data")
        new = deserialize(data["data"])
        if new is None:
            return self.respond_with_error("invalid data")
        State.set(new)
        self.sync()

    def save(self):
        cues = State.get()
        filename = "scenes.json"
        data = serialize(cues)
        data = json.dumps(data)
        with open(filename, "w") as f:
            f.write(data)
            self.send({"type": "success", "description": "Saved!"})
            return
        self.respond_with_error("error saving")

    def send(self, data: dict):
        self.write_message(json.dumps(data))

    def respond_with_error(self, msg: str):
        self.send({"type": "error", "description": msg})

    def on_close(self):
        pass
