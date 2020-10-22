import {Dispatch} from 'react';
import Action from './action';

const SERVER_URL: string = 'ws://localhost:8000';

type GetLevels = {
  type: 'get-levels',
  cue: {
    'current': string,
    'fade_time': string,
    'fade_progress': string,
    'fading': string,
    'last': string,
    'next': string,
    'previous': string,
  },
  values: [
    {
      'channel': string,
      'value': string,
      status: 'manual' | 'lowered' | 'raised' | 'tracked' | 'blocked';
    }
  ]
};

type ListCues = {
  type: 'list-cues',
  cue: string,
  cues: [
    {
      'number': string,
    }
  ]
};

type Message = GetLevels | ListCues;

export default class Server {
  socket: WebSocket;
  dispatch: Dispatch<Action>;

  constructor(dispatch: Dispatch<Action>) {
    this.dispatch = dispatch;
    this.socket = this.connect();
  }

  private connect() {
    if (this.socket && this.socket.readyState === WebSocket.OPEN) {
      return this.socket;
    }
    let socket = new WebSocket(SERVER_URL);
    socket.onmessage = (evt) => this.onmessage(evt.data);
    socket.onopen = () => this.onopen();
    socket.onclose = () => {
      this.dispatch({type: 'connection_change', status: false});
      setTimeout(() => {
        this.socket = this.connect();
      }, 1000);
    }
    socket.onerror = () => {
      socket.close();
    }
    this.socket = socket;
    return socket;
  }

  private onopen() {
    this.dispatch({type: 'connection_change', status: true});
    let data = {type: 'get-levels'};
    this.socket.send(JSON.stringify(data));
    data = {type: 'list-cues'};
    this.socket.send(JSON.stringify(data));
  }

  private onmessage(data: string) {
    let msg: Message = JSON.parse(data);
    switch (msg.type) {
      case 'get-levels':
        this.get_levels(msg);
        break;
      case 'list-cues':
        this.list_cues(msg);
        break;
      default:
        console.log("Unrecognized message type!");
    }
  }

  private get_levels(msg: GetLevels) {
    let values = msg.values || [];
    this.dispatch({
      type: 'update_channels',
      values: values.map((x) => {
        return {
          channel: parseInt(x.channel),
          value: parseInt(x.value),
          status: x.status,
        }
      }),
    });
    this.dispatch({
      type: 'update_cue',
      cue: {
        current: parseInt(msg.cue.current),
        fade_time: parseFloat(msg.cue.fade_time),
        fade_progress: parseFloat(msg.cue.fade_progress),
        fading: msg.cue.fading === "true",
        last: parseInt(msg.cue.last),
        next: parseInt(msg.cue.next),
        previous: parseInt(msg.cue.previous),
      }
    });
  }

  private list_cues(msg: ListCues) {
    let cues = msg.cues || [];
    this.dispatch({
      type: 'update_cue_list',
      cues: cues.map((x) => {
        return parseInt(x.number);
      }),
    });
  }

  reset_channel(channel: number) {
    channel = Math.floor(channel);
    let data = {
      type: 'reset-channel',
      channel,
    };
    this.socket.send(JSON.stringify(data));
  }

  block_channel(channel: number) {
    channel = Math.floor(channel);
    let data = {
      type: 'block-channel',
      channel,
    };
    this.socket.send(JSON.stringify(data));
  }

  track_channel(channel: number) {
    channel = Math.floor(channel);
    let data = {
      type: 'track-channel',
      channel,
    };
    this.socket.send(JSON.stringify(data));
  }

  set_level(channel: number, value: number) {
    channel = Math.floor(channel);
    value = Math.floor(value);
    let data = {
      type: 'set-levels',
      values: [
        {
          channel,
          value,
        }
      ]
    };
    this.socket.send(JSON.stringify(data));
  }

  save_cue(cue: number, time: number) {
    cue = Math.floor(cue);
    let data = {
      type: 'save-cue',
      cue,
      time,
    };
    this.socket.send(JSON.stringify(data));
  }

  restore_cue(cue: number) {
    cue = Math.floor(cue);
    let data = {
      type: 'restore-cue',
      cue,
    };
    this.socket.send(JSON.stringify(data));
  }

  go() {
    let data = {
      type: 'go-cue',
    };
    this.socket.send(JSON.stringify(data));
  }

  back() {
    let data = {
      type: 'back-cue',
    };
    this.socket.send(JSON.stringify(data));
  }
}
