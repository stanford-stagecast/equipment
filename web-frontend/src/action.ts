type Action =
  | { type: 'connection_success' }
  | { type: 'update_channel', channel: number, value: number }
  | { type: 'update_channels', values: {channel: number, value: number}[] };

export default Action;
