import React, {useState, useReducer, useEffect, useRef, MutableRefObject} from 'react';
import './App.css';
import reducer from '../../reducer';
import Fader, {FaderData} from '../Fader/Fader';
import Server from '../../server';
import CueStatus, {CueStatusData} from '../CueStatus/CueStatus';
import Controller from '../../controller';


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
  let [audioMode, setAudioMode] = useState(true);

  const server: MutableRefObject<Server | null> = useRef(null);
  const controller: MutableRefObject<Controller | null> = useRef(null);
  useEffect(() => {
    server.current = new Server(dispatch);
    controller.current = new Controller(initialState, dispatch, server.current as Server);
  }, []);

  if (server.current === null) {
    return <div>Loading...</div>
  } else if (!state.connected) {
    return <div>Connecting...</div>
  }
  controller.current?.update_state(state);


  const state_modified = state.faders.reduce((prev, current) => prev || current.status === "manual", false);

  return (
    <div className="App">
      <div className="controls">
        <label>
          Audio Mode:
          <input type="checkbox" checked={audioMode} onChange={(e) => setAudioMode(e.target.checked)}/>
        </label>
      </div>
      {
        !audioMode ?
        <>
          <div className="FaderBank">
            {
              state.faders.sort((a, b) => (a.channel > b.channel) ? 1 : -1).map((faderState: FaderData, i: number) => {
                return <Fader key={i} state_modified={state_modified} data={faderState} dispatch={dispatch} server={server.current as Server}/>
                })
            }
          </div>
          <CueStatus data={state.cue} dispatch={dispatch} server={server.current as Server}/>
        </>
        : <>
          <div className="ChannelList">
            {
              (() => {
                let n = state.faders.reduce((prev, curr) => curr.channel >= prev ? curr.channel : prev, 0);
                n = Math.ceil((n+1) / 16) * 16;
                const columns = 8;
                const rows = n / 16;
                let data = new Array(n).fill(0);
                for (let channel of state.faders) {
                  data[channel.channel] = channel.value;
                }
                let row_output = [];
                for (let row = 0; row < rows; row++) {
                  let column_output = [];
                  for (let column = 0; column < columns; column++) {
                    let level_index = row * 16 + column;
                    let pan_index = row * 16 + column + 8;
                    let level = data[level_index];
                    let pan = data[pan_index];
                    column_output.push(<span key={column} className="channel">
                      {row}.{level_index % 16}
                      <br/>
                      <progress className="pan" value={pan} max="255"></progress>
                      <progress className="intensity" value={level} max="255"></progress>
                    </span>);
                  }
                  row_output.push(<div key={row} data-row={row} className="row">{column_output}</div>);
                }
                return row_output;
              })()
            }
          </div>
        </> }
    </div>
  );
}
