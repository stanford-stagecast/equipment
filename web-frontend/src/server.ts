import {Dispatch} from 'react';
import Action from './action';

const SERVER_URL: string = 'ws://localhost:8000';

type GetLevels = {
  type: 'get-levels',
  list: string,
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

type GetLists = {
  type: 'get-lists',
  lists: [
    {
      'number': string,
      'name': string,
    }
  ]
};

type ListCues = {
  type: 'list-cues',
  list: string,
  cue: string,
  cues: [
    {
      'number': string,
    }
  ]
};

type Message = GetLevels | GetLists | ListCues;

export default class Server {
  socket: WebSocket;
  dispatch: Dispatch<Action>;
  disconnect_timeout?: number;
  list_id: number;

  constructor(dispatch: Dispatch<Action>, list_id: number) {
    this.dispatch = dispatch;
    this.socket = this.connect();
    this.list_id = list_id;
  }

  private connect() {
    if (this.socket && this.socket.readyState === WebSocket.OPEN) {
      return this.socket;
    }
    let socket = new WebSocket(SERVER_URL);
    socket.onmessage = (evt) => this.onmessage(evt.data);
    socket.onopen = () => this.onopen();
    socket.onclose = () => {
      if (this.disconnect_timeout === undefined) {
        this.disconnect_timeout = window.setTimeout(() => {
          this.dispatch({type: 'connection_change', status: false});
        }, 1000);
      }
      setTimeout(() => {
        this.socket = this.connect();
      }, 100);
    }
    socket.onerror = () => {
      socket.close();
    }
    this.socket = socket;
    return socket;
  }

  public disconnect() {
    console.log("disconnect");
    this.onmessage = () => {};
    if (this.socket) {
      this.socket.onclose = () => {};
      this.socket.close();
    }
  }

  private onopen() {
    window.clearTimeout(this.disconnect_timeout);
    this.disconnect_timeout = undefined;
    this.dispatch({type: 'connection_change', status: true});
    {
      let data = {type: 'get-levels', list_id: this.list_id};
      this.socket.send(JSON.stringify(data));
      data = {type: 'list-cues', list_id: this.list_id};
      this.socket.send(JSON.stringify(data));
    }
    {
      let data = {type: 'get-lists'};
      this.socket.send(JSON.stringify(data));
    }
  }

  private onmessage(data: string) {
    try {
      let msg: Message = JSON.parse(data);
      switch (msg.type) {
        case 'get-levels':
          this.get_levels(msg);
          break;
        case 'list-cues':
          this.list_cues(msg);
          break;
        case 'get-lists':
          this.get_lists(msg);
          break;
        default:
          console.log("Unrecognized message type!");
      }
    } catch {
      console.error("Error while deserializing!");
    }
  }

  private get_levels(msg: GetLevels) {
    if (parseInt(msg.list) !== this.list_id) return;
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

  private get_lists(msg: GetLists) {
    let lists = msg.lists || [];
    this.dispatch({
      type: 'update_lists',
      lists: lists.map((x) => {
        return {
          name: x.name,
          id: parseInt(x.number)
        }
      }),
    });
  }

  private list_cues(msg: ListCues) {
    if (parseInt(msg.list) !== this.list_id) return;
    let cues = msg.cues || [];
    this.dispatch({
      type: 'update_cue_list',
      cues: cues.map((x) => {
        return parseInt(x.number);
      }),
    });
  }

  rename_list(list_id: number, name: string) {
    list_id = Math.floor(list_id);
    let data = {
      type: 'rename-list',
      list_id: this.list_id,
      name,
    };
    this.socket.send(JSON.stringify(data));
  }

  delete_list(list_id: number) {
    list_id = Math.floor(list_id);
    let data = {
      type: 'delete-list',
      list_id: this.list_id,
    };
    this.socket.send(JSON.stringify(data));
  }

  reset_channel(channel: number) {
    channel = Math.floor(channel);
    let data = {
      type: 'reset-channel',
      list_id: this.list_id,
      channel,
    };
    this.socket.send(JSON.stringify(data));
  }

  block_channel(channel: number) {
    channel = Math.floor(channel);
    let data = {
      type: 'block-channel',
      list_id: this.list_id,
      channel,
    };
    this.socket.send(JSON.stringify(data));
  }

  track_channel(channel: number) {
    channel = Math.floor(channel);
    let data = {
      type: 'track-channel',
      list_id: this.list_id,
      channel,
    };
    this.socket.send(JSON.stringify(data));
  }

  set_level(channel: number, value: number) {
    channel = Math.floor(channel);
    value = Math.floor(value);
    let data = {
      type: 'set-levels',
      list_id: this.list_id,
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
      list_id: this.list_id,
      cue,
      time,
    };
    this.socket.send(JSON.stringify(data));
  }

  delete_cue(cue: number) {
    cue = Math.floor(cue);
    let data = {
      type: 'delete-cue',
      list_id: this.list_id,
      cue,
    };
    this.socket.send(JSON.stringify(data));
  }

  restore_cue(cue: number) {
    cue = Math.floor(cue);
    let data = {
      type: 'restore-cue',
      list_id: this.list_id,
      cue,
    };
    this.socket.send(JSON.stringify(data));
  }

  go() {
    let data = {
      type: 'go-cue',
      list_id: this.list_id,
    };
    this.socket.send(JSON.stringify(data));
  }

  back() {
    let data = {
      type: 'back-cue',
      list_id: this.list_id,
    };
    this.socket.send(JSON.stringify(data));
  }

  save_to_disk() {
    let data = {
      type: 'save-to-disk',
    };
    this.socket.send(JSON.stringify(data));
  }
}
