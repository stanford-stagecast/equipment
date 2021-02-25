from cue_list import CueList
from cue import Cue
from camera import Camera
from media import Media
from change import Change
from typing import Dict, Any, Optional


def serialize_change(change: Change) -> dict:
    return change.data


def serialize(cues: CueList) -> dict:
    data: Dict[str, Any] = {
        "cameras": [],
        "media": [],
        "cues": [],
    }

    for cameras in cues.cameras:
        data["cameras"] += [
            {
                "id": cameras.id,
                "name": cameras.name,
            }
        ]
    for media in cues.media:
        data["media"] += [
            {
                "name": media.name,
                "id": media.id,
                "file": media.file,
                "type": media.type,
            }
        ]

    for cue in cues.cues:
        data["cues"] += [
            {
                "number": cue.number,
                "name": cue.name,
                "changes": [
                    serialize_change(change) for change in cue.changes
                ]
            }
        ]
    return data


def deserialize(data: dict) -> Optional[CueList]:
    cameras = data.get("cameras", [])  # Can't change this
    media = data.get("media", [])  # Can't change this
    cues = data.get("cues", [])
    lst = CueList()
    numbers = set()

    for cue in cues:
        name = cue.get("name", "")
        number = cue.get("number")
        changes = cue.get("changes")
        if changes is None:
            return None
        if number is None:
            return None
        if number in numbers:
            return None
        numbers |= {number}
        changes = cue.get("changes", [])
        change_final = [Change(data) for data in changes]
        lst.cues += [Cue(number, name, change_final)]

    numbers = set()
    for camera in cameras:
        name = camera.get("name", "")
        id = camera.get("id")
        if id is None:
            return None
        if id in numbers:
            return None
        numbers |= {id}
        lst.cameras += [Camera(name, id)]

    numbers = set()
    for medium in media:
        name = medium.get("name", "")
        id = medium.get("id")
        file = medium.get("file", "")
        type = medium.get("type", "")
        if id is None:
            return None
        if id in numbers:
            return None
        if type not in ["audio", "video", "image"]:
            return None
        numbers |= {id}
        lst.media += [Media(name, id, file, type)]

    return lst
