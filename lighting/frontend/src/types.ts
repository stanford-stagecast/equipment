export type State = {
  channels: Channel[],
  cues: Cue[],
};

export type Channel = {
  name: string,
  number: number,
};

export type Cue = {
  time: number,
  number: number,
  name: string,
  deltas: Delta[],
}

export type Delta = {
  channel: number,
  intensity: number,
  hue: number,
};
