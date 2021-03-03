import json
import tornado.websocket
from cue_list import CueList
from serializer import serialize, deserialize
from state import State
from playback import Playback


class ClientHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        self.sync()
        Playback.add_listener(self)
        cue = Playback.get_current()
        data = {
            "type": "go",
            "data": {
                "cue": cue,
                "jump": True,
            }
        }
        self.write_message(json.dumps(data))

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
        elif tp == "go":
            if "data" not in data or "cue" not in data["data"]:
                return self.respond_with_error("no cue specified")
            Playback.set_current(data["data"]["cue"])
            return
        return self.respond_with_error("invalid request")

    def sync(self):
        cues = State.get()
        self.respond({"type": "sync", "data": serialize(cues)})

    def send(self, cue: int, jump: bool):
        data = {
            "type": "go",
            "data": {
                "cue": cue,
                "jump": jump,
            }
        }
        try:
            self.write_message(json.dumps(data))
        except tornado.WebSocketClosedError:
            Playback.remove_listener(self)
        self.sync()

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
            self.respond({"type": "success", "description": "Saved!"})
            return
        self.respond_with_error("error saving")

    def respond(self, data: dict):
        self.write_message(json.dumps(data))

    def respond_with_error(self, msg: str):
        self.respond({"type": "error", "description": msg})

    def on_close(self):
        Playback.remove_listener(self)
