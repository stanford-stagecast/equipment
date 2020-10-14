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
  faders: FaderData[];
}

/**
 * The initial state of the application.  This should probably put the
 * application in "Loading" state and wait for the server to provide the
 * initial data.
 */
export const initialState: AppState = {
  connected: false,
  faders: [
    {channel: 0, value: 2},
    {channel: 1, value: 4},
    {channel: 2, value: 8},
    {channel: 3, value: 16},
    {channel: 4, value: 32},
    {channel: 5, value: 64},
    {channel: 6, value: 128},
    {channel: 7, value: 255},
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

  return (
    <div className="App">
      <div className="FaderBank">
        {
          server.current === null
          ? 'Loading...'
          : state.faders.map((faderState: FaderData, i: number) => {
            return <Fader key={i} data={faderState} dispatch={dispatch} server={server.current as Server}/>
          })
        }
      </div>
    </div>
  );
}
