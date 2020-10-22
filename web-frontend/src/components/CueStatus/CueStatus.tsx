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

  useEffect(() => {
    if (data.current !== saved_cue_number) {
      set_saved_cue_number(data.current);
      set_cue_number(data.current);
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
          <input type="number" value={cue_number} onChange={(evt) => set_cue_number(parseInt(evt.target.value))}/>
          <button className="_record" onClick={() => server.save_cue(cue_number, 1)}>
            Record
          </button>
        </div>
      </div>
    </div>
  );
}
