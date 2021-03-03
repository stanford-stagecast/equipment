import json
import tornado.websocket
from cue_list import CueList
from serializer import serialize, deserialize
from playback import Playback


class LiveHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        Playback.add_listener(self)
        pass

    def on_message(self, message):
        cue = Playback.get_current()
        data = {
            "cue": cue,
            "jump": True,
        }
        self.write_message(json.dumps(data))

    def send(self, cue: int, jump: bool):
        data = {
            "cue": cue,
            "jump": jump,
        }
        try:
            self.write_message(json.dumps(data))
        except tornado.WebSocketClosedError:
            Playback.remove_listener(self)

    def on_close(self):
        Playback.remove_listener(self)
        pass
