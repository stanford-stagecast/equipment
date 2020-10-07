import React, {Dispatch} from 'react';
import Action from '../../action';
import './Fader.css';

/**
 * The globally-saved state of a fader (anything which should be saved on the
 * server or visible to other components).  Local-only state may be saved or
 * accessed via the `useState` hook instead.
 */
export type FaderData = {
  channel: number,
  value: number,
}

/**
 * Properties passed down from the parent into the fader.  These must be
 * updated via `dispatch`.
 */
type FaderProps = {
  data: FaderData
  dispatch: Dispatch<Action>;
}

/**
 * Represents a basic one-channel input (e.g. a single channel's volume or a
 * single dimmer's intensity).
 */
export default function Fader({data, dispatch}: FaderProps) {
  return (
    <div className="Fader">
      Fader {data.channel} is at {data.value} ({Math.round(data.value/255 * 100)}%).
      <br/>
      <input
        type="range"
        min="0"
        max="255"
        value={data.value}
        onChange={(event) => fader_changed(event.target, data, dispatch)}/>
    </div>
  );
}

/**
 * A fader was moved; dispatch the appropriate events to update the state.
 */
function fader_changed(target: HTMLInputElement, data: FaderData, dispatch: Dispatch<Action>) {
  /* We'd probably want:
   * update_channel_request (update local state)
   * send the data to the backend, then:
   * update_channel_success (sync the local state)
   * or:
   * update_channel_fail (alert the user of the failure).
   */
  dispatch({
    type: 'update_channel',
    channel: data.channel,
    value: parseInt(target.value),
  });
}

