import React, {useReducer, useEffect, useRef, MutableRefObject} from 'react';
import './App.css';
import reducer from '../../reducer';
import Fader, {FaderData} from '../Fader/Fader';
import Server from '../../server';
import WebMidi from 'webmidi';
import CueStatus, {CueStatusData} from '../CueStatus/CueStatus';


/**
 * The application's overall state.  Every piece of server-side state the application needs
 * is also saved in here somewhere, plus any top-level local state.  This
 * should probably be a union of "Loading", "Active", and "Error" states.
 */
export type AppState = {
  connected: boolean;
  cue: CueStatusData,
  cues: number[],
  faders: FaderData[];
}

/**
 * The initial state of the application.  This should probably put the
 * application in "Loading" state and wait for the server to provide the
 * initial data.
 */
export const initialState: AppState = {
  connected: false,
  cue: {
    current: 0,
    fade_time: 1,
    fade_progress: 1,
    fading: false,
    last: 0,
    next: 0,
    previous: 0,
  },
  cues: [],
  faders: [
    {channel: 0, value: 0, status: 'manual'},
    {channel: 1, value: 0, status: 'manual'},
    {channel: 2, value: 0, status: 'manual'},
    {channel: 3, value: 0, status: 'manual'},
    {channel: 4, value: 0, status: 'manual'},
    {channel: 5, value: 0, status: 'manual'},
    {channel: 6, value: 0, status: 'manual'},
    {channel: 7, value: 0, status: 'manual'},
  ]
}

/**
 * The Stagecast CueLab Frontend Application.  The entire application is
 * contained under here.
 */
export default function App(_props: {}) {
  const [state, dispatch] = useReducer(reducer, initialState);

  const server: MutableRefObject<Server | null> = useRef(null);
  useEffect(() => {
    server.current = new Server(dispatch);
  }, []);

  if (server.current === null) {
    return <div>Loading...</div>
  } else if (!state.connected) {
    return <div>Connecting...</div>
  }

  //midi_init();
  WebMidi.enable(function(err) {
    if (err) {
      console.log('WebMidi is not supported');
    } else {
      console.log('WebMidi enabled');
      navigator.requestMIDIAccess({sysex: true}).then(function(midi_access){
        for (let input of midi_access.inputs.values()) {
          input.onmidimessage = function(midi_message){
            // state.faders[midi_message.data[1]].value = midi_message.data[2];
            // console.log(state.faders[midi_message.data[1]]);
            dispatch({type: 'update_channel', value: {channel: midi_message.data[1], value: 2 * midi_message.data[2], status: 'manual'}});;
          };
        }
      });
    }
  });

  const state_modified = state.faders.reduce((prev, current) => prev || current.status === "manual", false);

  return (
    <div className="App">
      <div className="FaderBank">
        {
          state.faders.sort((a, b) => (a.channel > b.channel) ? 1 : -1).map((faderState: FaderData, i: number) => {
            return <Fader key={i} state_modified={state_modified} data={faderState} dispatch={dispatch} server={server.current as Server}/>
          })
        }
      </div>
      <br/>
      <CueStatus data={state.cue} dispatch={dispatch} server={server.current as Server}/>
    </div>
  );
}
