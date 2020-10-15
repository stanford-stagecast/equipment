import {Dispatch} from 'react';
import Action from './action';

const SERVER_URL: string = 'ws://localhost:8000';

type GetLevels = {
  type: 'get-levels',
  cue: string,
  values: [
    {
      'channel': string,
      'value': string,
      'status': 'saved' | 'changed',
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
    this.socket = new WebSocket(SERVER_URL);
    this.dispatch = dispatch;
    this.socket.onmessage = (evt) => this.onmessage(evt.data);
    this.socket.onopen = () => this.onopen();
  }

  private onopen() {
    this.dispatch({type: 'connection_success'});
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
      cue: parseInt(msg.cue),
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
    this.dispatch({
      type: 'update_cue',
      cue: parseInt(msg.cue),
    });
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

  save_cue(cue: number) {
    cue = Math.floor(cue);
    let data = {
      type: 'save-cue',
      cue: cue
    };
    this.socket.send(JSON.stringify(data));
  }

  restore_cue(cue: number) {
    cue = Math.floor(cue);
    let data = {
      type: 'restore-cue',
      cue: cue
    };
    this.socket.send(JSON.stringify(data));
  }
}
