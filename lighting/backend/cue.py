from delta import Delta
from typing import Optional, List


class Cue:
    def __init__(
        self, number: int, name: str, time: int, deltas: Optional[List[Delta]]
    ):
        self.number = number
        self.name = name
        self.time = time
        self.deltas = deltas or []
