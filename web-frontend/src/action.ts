export type Channel = {
  channel: number,
  value: number,
  status: 'saved' | 'changed';
};

type Action =
  | { type: 'connection_success' }
  | { type: 'update_cue', cue: number }
  | { type: 'update_cue_list', cues: number[] }
  | { type: 'update_channel', value: Channel}
  | { type: 'update_channels', values: Channel[] };

export default Action;
