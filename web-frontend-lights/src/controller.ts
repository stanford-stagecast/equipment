import {Dispatch} from 'react';
import Action from './action';
import Server from './server';
import {AppState} from './components/App/App';
// import WebMidi from 'webmidi';

export default class Controller {
  server: Server;
  dispatch: Dispatch<Action>;
  input?: WebMidi.MIDIInput;
  timeouts: Map<number, number>;
  pending_recording: boolean;
  last_state: AppState;

  constructor(state: AppState, dispatch: Dispatch<Action>, server: Server) {
    this.last_state = state;
    this.dispatch = dispatch;
    this.server = server;
    this.connect();
    this.timeouts = new Map();
    this.pending_recording = false;
  }

  private connect() {
    if (!navigator.requestMIDIAccess) {
      console.log('WebMidi is not supported');
      return;
    }
    console.log('WebMidi enabled');
    navigator.requestMIDIAccess({sysex: true}).then((a) => this.onaccess(a), () => this.onfailure());
  }

  private onaccess(access: WebMidi.MIDIAccess) {
    let inputs = access.inputs;
    if (inputs.size === 0) {
      console.log('No MIDI devices detected.');
    }
    for (let entry of inputs.entries()) {
      let input = entry[1];
      if (!input.manufacturer) continue;
      this.input = input;
      input.onmidimessage = (m: WebMidi.MIDIMessageEvent) => this.onmessage(m);
      return;
    }
  }

  private onfailure() {
    console.log('WebMidi access denied');
  }

  private onmessage(message: WebMidi.MIDIMessageEvent) {
    let channel = message.data[1];
    let value = message.data[2]*2;
    if (channel > 8) {
      if (value === 0) {
        return;
      }
      if (this.pending_recording) {
        if (channel === 45 || channel === 60) {
          // record + record or record + marker set
          this.server.save_cue(this.last_state.cue.current, this.last_state.cue.fade_time);
        } else if (channel === 61) {
          // record + marker left
          this.server.save_cue(this.last_state.cue.current - 1, this.last_state.cue.fade_time);
        } else if (channel === 62) {
          // record + marker right
          this.server.save_cue(this.last_state.cue.current + 1, this.last_state.cue.fade_time);
        }
        this.pending_recording = false;
      } else {
        if (channel === 41 || channel === 44) {
          // play or forward
          this.server.go();
        }
        if (channel === 42 || channel === 43) {
          // stop or back
          this.server.back();
        }
        if (channel === 45) {
          // record
          this.pending_recording = true;
        }
        if (channel >= 64 && channel <= 71) {
          // R buttons
          this.server.reset_channel(channel - 64);
        }
      }
      return;
    }
    // this.dispatch({type: 'update_channel', value: {channel: channel, value: value, status: 'manual'}});
    if (this.timeouts.has(channel)) {
      clearTimeout(this.timeouts.get(channel));
    }
    this.timeouts.set(channel,
      window.setTimeout(() => this.ontimeout(channel, value), 1)
    );
  }

  private ontimeout(channel: number, value: number) {
    this.server.set_level(channel, value);
    this.timeouts.delete(channel);
  }

  public update_state(state: AppState) {
    this.last_state = state;
  }

}
