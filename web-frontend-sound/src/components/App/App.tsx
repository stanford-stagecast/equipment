import React, {useReducer, useEffect, useRef, MutableRefObject} from 'react';
import './App.css';
import reducer from '../../reducer';
import Fader, {FaderData} from '../Fader/Fader';
import Server from '../../server';
import CueStatus, {CueStatusData} from '../CueStatus/CueStatus';
import Controller from '../../controller';
import ListControls, {ListData} from '../ListControls/ListControls';


/**
 * The application's overall state.  Every piece of server-side state the application needs
 * is also saved in here somewhere, plus any top-level local state.  This
 * should probably be a union of "Loading", "Active", and "Error" states.
 */
export type AppState = {
  connected: boolean,
  cue: CueStatusData,
  cues: number[],
  faders: FaderData[],
  lists: ListData,
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
    {channel: 0, value: 128, mute: true, status: 'manual'},
  ],
  lists: {
    id: 0,
    name: "N/A",
    lists: [],
  }
}

export default function App(_props: {}) {
  const [state, dispatch] = useReducer(reducer, initialState);

  const server: MutableRefObject<Server | null> = useRef(null);
  const controller: MutableRefObject<Controller | null> = useRef(null);
  useEffect(() => {
    server.current = new Server(dispatch, state.lists.id);
    return () => {
      server.current?.disconnect();
    }
  }, [state.lists.id]);

  useEffect(() => {
    if (server.current) {
      controller.current = new Controller(initialState, dispatch, server.current as Server);
    }
  }, [server]);

  if (server.current === null) {
    return <div>Loading...</div>
  } else if (!state.connected) {
    return <div>Connecting...</div>
  }
  controller.current?.update_state(state);


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
      <button onClick={() => show_channel(server.current as Server)}>Show Hidden Channel</button>
      <CueStatus data={state.cue} cues={state.cues} dispatch={dispatch} server={server.current as Server}/>
      <button onClick={() => server.current?.save_to_disk()}>Save to Disk</button>
    </div>
  );
}

function show_channel(server: Server) {
  let channel = prompt("Enter a channel number to enable");
  if (!channel) {
    return;
  }
  let num = parseInt(channel);
  if (num === undefined) {
    return;
  }
  server.show_channel(num);
}
