import React, {Dispatch, useState, useEffect} from 'react';
import Action, {CueStatusData as ActionCueStatusData} from '../../action';
import './CueStatus.css';
import Server from '../../server';

export type CueStatusData = ActionCueStatusData;

/**
 * Properties passed down from the parent into the fader.  These must be
 * updated via `dispatch`.
 */
type CueStatusProps = {
  data: CueStatusData;
  dispatch: Dispatch<Action>;
  server: Server;
}

/**
 * Represents a basic one-channel input (e.g. a single channel's volume or a
 * single dimmer's intensity).
 */
export default function CueStatus({data, dispatch, server}: CueStatusProps) {
  let [cue_number, set_cue_number] = useState(data.current);
  let [saved_cue_number, set_saved_cue_number] = useState(data.current);
  let [cue_time, set_cue_time] = useState(data.fade_time);

  useEffect(() => {
    if (data.current !== saved_cue_number) {
      set_saved_cue_number(data.current);
      set_cue_number(data.current);
      set_cue_time(data.fade_time);
    }
  }, [data, saved_cue_number]);

  return (
    <div className="CueStatus">
      <div className="_view">
        <span className="_previousQ">{data.previous}&larr;</span>
        <span className="_currentQ">{data.current}</span>
        <span className="_nextQ">&rarr;{data.next}</span>
      </div>
      <progress className="_fadeProgress" max="255" value={data.fade_progress * 255}/>
      <div className="_controls">
        <button className="_back"
          onClick={() => server.back()}
          disabled={data.previous === data.current}>
          Back
        </button>
        <button className="_go"
          onClick={() => server.go()}
          disabled={!data.fading && data.next === data.current}>
          Go!
        </button>
        <div className="_recording">
          <label>
            Cue:
            <input type="number" value={cue_number} onChange={(evt) => set_cue_number(parseInt(evt.target.value) || 0)}/>
          </label>
          <label>
            Time:
            <input type="number" value={cue_time} onChange={(evt) => set_cue_time(parseFloat(evt.target.value) || 0)}/>
          </label>
          <button className="_record" onClick={() => server.save_cue(cue_number, cue_time)}>
            Record
          </button>
          <button className="_record" onClick={() => delete_cue(cue_number, server)}>
            Delete
          </button>
        </div>
      </div>
    </div>
  );
}

function delete_cue(q: number, server: Server) {
  let confirmation = window.confirm(`Are you sure you want to delete cue #${q}?`);
  if (confirmation === true) {
    server.delete_cue(q);
  }
}
