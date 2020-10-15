import Action, {Channel} from './action';
import {AppState} from './components/App/App';

/**
 * Update the application's client-side global state.  Call via `dispatch`.
 */
export default function reducer(state: AppState, action: Action): AppState {
  switch (action.type) {
    case 'connection_success':
      return {...state, connected: true};
    case 'update_cue':
      return {...state, cue: action.cue};
    case 'update_cue_list':
      return {...state, cues: action.cues};
    case 'update_channel':
      return update_channel(state, action.value);
    case 'update_channels':
      return update_channels(state, action.values);
  }
}

/**
 * Updates the application's client-side global state when a channel is changed.
 */
function update_channel(state: AppState, value: Channel): AppState {
  if (value.value < 0 || value.value > 255) return state;
  let faders = state.faders.map((fader) => {
    if (fader.channel === value.channel) {
      return {
        channel: fader.channel,
        value: value.value,
        status: value.status,
      }
    } else {
      return fader;
    }
  });
  return {
    ...state,
    faders,
  };
}

function update_channels(state: AppState, values: Channel[]): AppState {
  for (let value of values) {
    state = update_channel(state, value)
  }
  return state;
}
