from state import State
from typing import Set

cue = 0
listeners: Set["LiveHandler"] = set()


class Playback:
    @staticmethod
    def get_current() -> int:
        global cue
        return cue

    @staticmethod
    def set_current(q: int):
        global cue
        state = State.get()
        if state is None:
            return
        cues = [cue.number for cue in state.cues]
        jump = True
        try:
            i = cues.index(cue)
        except ValueError:
            i = -100

        try:
            j = cues.index(q)
        except ValueError:
            pass
        if i == j:
            return
        if i == j - 1:
            jump = False
        cue = q
        for listener in list(listeners):
            listener.send(cue, jump)

    @staticmethod
    def add_listener(listener):
        global listeners
        listeners |= {listener}

    @staticmethod
    def remove_listener(listener):
        global listeners
        listeners.remove(listener)
