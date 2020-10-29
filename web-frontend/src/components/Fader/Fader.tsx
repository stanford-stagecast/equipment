import React, {Dispatch} from 'react';
import Action, {Channel} from '../../action';
import './Fader.css';
import Server from '../../server';

/**
 * The globally-saved state of a fader (anything which should be saved on the
 * server or visible to other components).  Local-only state may be saved or
 * accessed via the `useState` hook instead.
 */
export type FaderData = Channel;

/**
 * Properties passed down from the parent into the fader.  These must be
 * updated via `dispatch`.
 */
type FaderProps = {
  state_modified: boolean;
  data: FaderData;
  dispatch: Dispatch<Action>;
  server: Server;
}

/**
 * Represents a basic one-channel input (e.g. a single channel's volume or a
 * single dimmer's intensity).
 */
export default function Fader({state_modified, data, dispatch, server}: FaderProps) {
  return (
    <div className="Fader">
      <p className={"_text _" + data.status}>
        {data.channel}: {Math.round(data.value/255 * 100)}
      </p>
      <br/>
      <input
        className="_fader"
        type="range"
        min="0"
        max="255"
        value={data.value}
        onChange={(event) => fader_changed(event.target, data, server, dispatch)}/>
      <button
        disabled={data.status !== "manual"}
        onClick={() => server.reset_channel(data.channel)}>
        Reset
      </button>
      <button
        disabled={state_modified || data.status === "tracked"}
        onClick={() => server.track_channel(data.channel)}>
        Track
      </button>
      <button
        disabled={state_modified || data.status !== "tracked"}
        onClick={() => server.block_channel(data.channel)}>
        Block
      </button>
    </div>
  );
}

/**
 * A fader was moved; dispatch the appropriate events to update the state.
 */
function fader_changed(target: HTMLInputElement, data: FaderData, server: Server, dispatch: Dispatch<Action>) {
  server.set_level(data.channel, parseInt(target.value));
  dispatch({
    type: 'update_channel',
    value: {
      ...data,
      value: parseInt(target.value),
    }
  });
}
