from cue_list import CueList
from cue import Cue
from delta import Delta
from typing import Dict, Any, Optional


def serialize(cues: CueList) -> dict:
    data: Dict[str, Any] = {
        "channels": [],
        "cues": [],
    }

    for channel in cues.channels:
        data["channels"] += [
            {
                "number": channel.number,
                "name": channel.name,
            }
        ]

    for cue in cues.cues:
        data["cues"] += [
            {
                "time": cue.time,
                "number": cue.number,
                "name": cue.name,
                "deltas": [
                    {"channel": delta.channel.number, "intensity":
                        delta.intensity, "hue": delta.hue}
                    for delta in cue.deltas
                ],
            }
        ]
    return data


def deserialize(data: dict) -> Optional[CueList]:
    _ = data.get("channels", [])  # Can't change this
    cues = data.get("cues", [])
    lst = CueList()
    channels = {c.number: c for c in lst.channels}
    numbers = set()
    for cue in cues:
        name = cue.get("name", "")
        number = cue.get("number")
        time = cue.get("time")
        if time is None:
            return None
        if number is None:
            return None
        if number in numbers:
            return None
        numbers |= {number}
        dels = cue.get("deltas", [])
        deltas = []
        for d in dels:
            channel = d.get("channel")
            intensity = d.get("intensity")
            hue = d.get("hue")
            if channel is None or intensity is None or hue is None:
                return None
            intensity = max(0, min(intensity, 255))
            hue = max(0, min(hue, 255))
            chan = channels.get(channel)
            if chan is None:
                return None
            deltas += [
                Delta(chan, intensity, hue)
            ]
        lst.cues += [Cue(number, name, time, deltas)]
    return lst
