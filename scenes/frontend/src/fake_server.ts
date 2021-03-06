import { writable } from 'svelte/store';
import type { State } from './types';
import type { Message } from './message';

let initial_state: State = {
  cameras: [
    {
      id: 0,
      name: "Close Up",
    },
    {
      id: 1,
      name: "Wide Angle",
    }
  ],
  media: [
    {
      name: "Stanford",
      id: 0,
      file: "stanford.png",
      type: "image",
    },
    {
      name: "Transition",
      id: 1,
      file: "transition.mp4",
      type: "video",
    },
    {
      name: "Doorbell",
      id: 2,
      file: "doorbell.wav",
      type: "audio",
    }
  ],
  cues: [
    {
      number: 0,
      name: "Beginning",
      changes: [
        {
          type: "media",
          time: 0,
          media_id: 0,
          action: {
            type: "add",
            config: {
              view: {
                top: 0.0,
                right: 0.0,
                bottom: 720.0,
                left: 1280.0,
                z: 1.0,
              },
              playback: {
                volume: 0.0,
                playing: true,
                done: "loop",
              }
            }
          }
        },
        {
          type: "camera",
          time: 0,
          camera_id: 0,
          action: {
            type: "add",
            config: {
              view: {
                top: 0.0,
                right: 0.0,
                bottom: 720.0,
                left: 1280.0,
                z: 0.0,
              }
            }
          }
        },
        {
          type: "linked",
          cue_type: "light",
          cue_number: 1000,
        }
      ],
    },
    {
      number: 100,
      name: "First",
      changes: [
        {
          type: "camera",
          time: 0,
          camera_id: 0,
          action: {
            type: "remove",
          }
        },
        {
          type: "media",
          time: 0,
          media_id: 1,
          action: {
            type: "add",
            config: {
              view: {
                top: 0.0,
                right: 0.0,
                bottom: 720.0,
                left: 640.0,
                z: 1.0,
              },
              playback: {
                volume: 0.0,
                playing: true,
                done: "loop",
              }
            }
          }
        }
      ],
    },
    {
      number: 300,
      name: "Last",
      changes: [
        {
          type: "media",
          time: 0,
          media_id: 1,
          action: {
            type: "remove",
          }
        }
      ],
    }
  ],
};

const fakeStore = writable(initial_state);

const sendMessage = (message: Message) => {
  console.log(message);
}

export default {
  subscribe: fakeStore.subscribe,
  sendMessage,
}
