import React, {Dispatch} from 'react';
import Action, {Channel} from '../../action';
import './Fader.css';
import Server from '../../server';
import WebMidi from 'webmidi';

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
  data: FaderData;
  dispatch: Dispatch<Action>;
  server: Server;
}

/**
 * Represents a basic one-channel input (e.g. a single channel's volume or a
 * single dimmer's intensity).
 */
export default function Fader({data, dispatch, server}: FaderProps) {
  return (
    <div className="Fader">
      <p className={"_text" + (data.status === 'changed' ? ' _changed' : '')}>
        {Math.round(data.value/255 * 100)}
      </p>
      <br/>
      <input
        className="_fader"
        type="range"
        min="0"
        max="255"
        value={data.value}
        onChange={(event) => fader_changed(event.target, data, server, dispatch)}/>
    </div>
  );
}

/**
 * A fader was moved; dispatch the appropriate events to update the state.
 */
function fader_changed(target: HTMLInputElement, data: FaderData, server: Server, dispatch: Dispatch<Action>) {
  /* We'd probably want:
   * update_channel_request (update local state)
   * send the data to the backend, then:
   * update_channel_success (sync the local state)
   * or:
   * update_channel_fail (alert the user of the failure).
   */
  server.set_level(data.channel, parseInt(target.value));
  dispatch({
    type: 'update_channel',
    value: {
      ...data,
      value: parseInt(target.value),
    }
  });
}
