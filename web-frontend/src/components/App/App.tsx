import React, {useReducer, useEffect, useRef, MutableRefObject} from 'react';
import './App.css';
import reducer from '../../reducer';
import Fader, {FaderData} from '../Fader/Fader';
import Server from '../../server';

/**
 * The application's overall state.  Every piece of server-side state the application needs
 * is also saved in here somewhere, plus any top-level local state.  This
 * should probably be a union of "Loading", "Active", and "Error" states.
 */
export type AppState = {
  connected: boolean;
  cue: number,
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
  cue: 0,
  cues: [],
  faders: [
    {channel: 0, value: 0, status: 'changed'},
    {channel: 1, value: 0, status: 'changed'},
    {channel: 2, value: 0, status: 'changed'},
    {channel: 3, value: 0, status: 'changed'},
    {channel: 4, value: 0, status: 'changed'},
    {channel: 5, value: 0, status: 'changed'},
    {channel: 6, value: 0, status: 'changed'},
    {channel: 7, value: 0, status: 'changed'},
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
    return <div>"Loading..."</div>
  }

  return (
    <div className="App">
      <div className="FaderBank">
        {
          state.faders.map((faderState: FaderData, i: number) => {
            return <Fader key={i} data={faderState} dispatch={dispatch} server={server.current as Server}/>
          })
        }
      </div>
      <br/>
      <div className="CueList">
        Current Cue: {state.cue}
        <br/>
        <button onClick={(event) => (server.current as Server).save_cue(state.cue)}>
          Save as {state.cue} (Current)
        </button>
        <br/>
        <button onClick={(event) => (server.current as Server).save_cue(state.cue + 1)}>
          Save as {state.cue + 1} (Next)
        </button>
        <br/>
        Cue List: {
          state.cues ? state.cues.map((cue) => {
            return <button onClick={(event) => (server.current as Server).restore_cue(cue)}>
              {cue}
            </button>;
          }) : "No saved cues."
        }
      </div>
    </div>
  );
}
