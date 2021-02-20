export const WIDTH = 1280;
export const HEIGHT = 720;

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
  view: ViewConfig, // ignore for audio
  playback: PlaybackConfig,
}

export type CameraConfig = {
  view: ViewConfig,
}

export type ViewConfig = {
  top: number, // y coordinate of the top of the media in the output
  right: number, // x coordinate of the right of the media in the output
  bottom: number, // y coordinate of the bottom of the media in the output
  left: number, // x coordinate of the left of the media in the output
  z: number, // z index of this media, higher = in front
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
