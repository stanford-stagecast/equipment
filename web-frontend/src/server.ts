import React, {Dispatch} from 'react';
import Action from './action';
import {parse} from 'path';

const SERVER_URL: string = 'ws://localhost:8000';

type GetLevels = {
  type: 'get-levels',
  cue: string,
  values: [
    {
      'channel': string,
      'value': string,
      'status': string,
    }
  ]
};

type ListCues = {
  type: 'list-cues',
  cue: string,
  values: [
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
    this.socket.onopen = (evt) => this.onopen();
  }

  private onopen() {
    this.dispatch({type: 'connection_success'});
    let data = {type: 'get-levels'};
    this.socket.send(JSON.stringify(data));
  }

  private onmessage(data: string) {
    let msg: Message = JSON.parse(data);
    switch (msg.type) {
      case 'get-levels':
        this.dispatch({
          type: 'update_channels',
          values: msg.values.map((x) => {
            return {
              channel: parseInt(x.channel),
              value: parseInt(x.value),
            }
          }),
        });
        break;
      default:
        console.log("Unrecognized message type!");
    }
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
}
