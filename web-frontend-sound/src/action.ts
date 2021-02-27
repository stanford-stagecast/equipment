export type Channel = {
  channel: number,
  value: number,
  mute: boolean,
  status: 'manual' | 'lowered' | 'raised' | 'tracked' | 'blocked';
};

export type CueList = {
  name: string,
  id: number,
};

export type CueStatusData = {
  current: number,
  fade_time: number,
  fade_progress: number,
  fading: boolean,
  last: number,
  next: number,
  previous: number,
};

type Action =
  | { type: 'connection_change', status: boolean }
  | { type: 'update_cue', cue: CueStatusData }
  | { type: 'update_cue_list', cues: number[] }
  | { type: 'update_channel', value: Channel}
  | { type: 'update_channels', values: Channel[] }
  | { type: 'update_lists', lists: CueList[] }
  | { type: 'switch_list', id: number };

export default Action;
