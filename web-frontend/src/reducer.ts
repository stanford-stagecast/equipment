import Action from './action';
import {AppState} from './components/App/App';

/**
 * Update the application's client-side global state.  Call via `dispatch`.
 */
export default function reducer(state: AppState, action: Action): AppState {
  console.log(action);
  switch (action.type) {
    case 'connection_success':
      return {...state, connected: true};
    case 'update_channel':
      return update_channel(state, action.channel, action.value);
    case 'update_channels':
      return update_channels(state, action.values);
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

function update_channels(state: AppState, values: {channel: number, value: number}[]): AppState {
  for (let value of values) {
    state = update_channel(state, value.channel, value.value)
  }
  return state;
}
