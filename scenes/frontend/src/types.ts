export type State = {
  cameras: Camera[],
  media: Media[],
  cues: Cue[],
};

export type Camera = {
  name: string
  id: number,
};

export type Media = {
  name: string,
  id: number,
  file: string,
  type: "video" | "audio" | "image",
};

export type Cue = {
  number: number, // actually (number * 100) to avoid floating point issues
  name: string,
  changes: Change[],
}

export type Change = MediaChange | CameraChange | LinkedCueChange;

export type MediaChange = {
  type: "media",
  time: number, // seconds
  media_id: number,
  action: AddMediaAction | UpdateMediaAction | RemoveMediaAction;
}

export type CameraChange = {
  type: "camera",
  time: number, // seconds
  camera_id: number,
  action: AddCameraAction | UpdateCameraAction | RemoveCameraAction;
}

export type LinkedCueChange = {
  type: "linked",
  cue_type: "light" | "sound",
  cue_number: number, // actually the integer (number * 100)
};

export type AddMediaAction = {
  type: "add",
  config: MediaConfig,
}

export type UpdateMediaAction = {
  type: "update",
  config: MediaConfig,
}

export type RemoveMediaAction = {
  type: "remove",
}

export type AddCameraAction = {
  type: "add",
  config: CameraConfig,
}

export type UpdateCameraAction = {
  type: "update",
  config: CameraConfig,
}

export type RemoveCameraAction = {
  type: "remove",
}

export type MediaConfig = {
  view: ViewConfig,
  playback: PlaybackConfig,
}

export type CameraConfig = {
  view: ViewConfig,
}

export type ViewConfig = {
  zoom: number, // 1 = 100%, 2 = 200%, 0.5 = 50%, etc.
  x: number, // x coordinate of the left side of the media: 0 = left, 1 = right
  y: number, // y coordinate of the right side of the media: 0 = top, 1 = bottom
  z: number, // z index of this media, higher = in front (ignore for audio)
}

export type PlaybackConfig = {
  volume: number; // 0 = silent, 1 = default, 2 = 2x (ignore for images)
  playing: boolean; // true = play, false = pause (ignore for images)
  done: "loop" | "stop" | "hold", // ignore for images
  /*
   *  loop: start over
   *  stop: remove media from output
   *  hold: freeze on last frame (not valid for audio)
   */
}
