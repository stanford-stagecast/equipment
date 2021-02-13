from typing import List

from cue import Cue
from channel import Channel


class CueList():
    def __init__(self):
        self.channels = Channel.get_defaults()
        self.cues = []
