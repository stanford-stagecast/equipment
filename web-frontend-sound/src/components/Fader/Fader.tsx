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
  let block = (
    <div className="Fader">
        <button
          onClick={() => hide_channel(data.channel, server)}>
          Hide
        </button>
		<label
		className="_mute">
		<input
          id={"mute"+data.channel.toString()}
		  type="checkbox"
          // checked={data.mute}
		  onChange={(event) => mute_toggled(event.target, data, server, dispatch)}
          />
		  <span className="_mute_box"><p>Mute</p></span>
		</label>
	  <div>
	  <span id="_pan_span">
	  <label id="_detent"><span id="_pan_detent"/></label>
	  <input
	  	className="_pan"
		type="range"
		min="0"
		max="255"
		value={data.value}
		onChange={(event) => level_changed(event.target, data, server, dispatch)}/>
	  </span>
	  </div>
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
  return block;
}

/**
 * A fader was moved; dispatch the appropriate events to update the state.
 */
function level_changed(target: HTMLInputElement, data: FaderData, server: Server, dispatch: Dispatch<Action>) {
  server.set_level(data.channel, parseInt(target.value), data.mute);
  dispatch({
    type: 'update_channel',
    value: {
      ...data,
      value: parseInt(target.value),
      mute: data.mute
    }
  });
}

function mute_toggled(target: HTMLInputElement, data: FaderData, server: Server, dispatch: Dispatch<Action>) {
    data.mute = target.checked;
    server.set_level(data.channel, data.value, target.checked);
    dispatch({
      type: 'update_channel',
      value: {
        ...data,
        value: parseInt(target.value),
        mute: data.mute
      }
    });
}

function hide_channel(channel: number, server: Server) {
  let confirmation = window.confirm(`Disabling channel ${channel} will hide it and turn off its output (over DMX or into the mixer). Please confirm.`);
  if (confirmation) {
    server.hide_channel(channel);
  }
}
