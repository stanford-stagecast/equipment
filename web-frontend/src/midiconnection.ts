import {Dispatch} from 'react';
import WebMidi from 'webmidi';



export default class MIDIConnection {
	midi_access: object;
	dispatch: Dispatch<Action>;

	constructor(dispatch: Dispatch<Action>) {
		navigator.requestMIDIAccess({sysex: true}).then(
			function(midi_access) { this.midi_access = midi_access; },
			function() { console.log('midi not supported'); }
		);
	}
}

// WebMidi.enable(function(err) {
//   if (err) {
// 	console.log('WebMidi is not supported');
//   } else {
// 	console.log('WebMidi enabled');
// 	navigator.requestMIDIAccess({sysex: true}).then(function(midi_access){
// 	  for (let input of midi_access.inputs.values()) {
// 		input.onmidimessage = function(midi_message){
// 		  // state.faders[midi_message.data[1]].value = midi_message.data[2];
// 		  // console.log(state.faders[midi_message.data[1]]);
// 		  dispatch({type: 'update_channel', value: {channel: midi_message.data[1], value: 2 * midi_message.data[2], status: 'changed'}});;
// 		};
// 	  }
// 	});
//   }
// });
