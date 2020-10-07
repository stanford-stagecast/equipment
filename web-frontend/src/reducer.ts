import Action from './action';
import {AppState} from './components/App/App';

/**
 * Update the application's client-side global state.  Call via `dispatch`.
 */
export default function reducer(state: AppState, action: Action): AppState {
  if (action.type === 'update_channel') {
    return update_channel(state, action.channel, action.value);
  } else {
    throw Error(`Action ${action.type} unrecognized!`);
  }
}

/**
 * Updates the application's client-side global state when a channel is changed.
 */
function update_channel(state: AppState, channel: number, value: number): AppState {
  if (value < 0 || value > 255) return state;
  console.log(`Channel ${channel} was updated!`);
  let faders = state.faders.map((fader) => {
    if (fader.channel === channel) {
      return {
        ...fader,
        value
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
