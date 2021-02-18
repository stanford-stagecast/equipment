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
      name: "stanford.png",
      id: 0,
      file: "stanford.png",
      type: "image",
    },
    {
      name: "hamilton.mp4",
      id: 1,
      file: "hamilton.mp4",
      type: "video",
    },
    {
      name: "doorbell.wav",
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
                x: 0.0,
                y: 0.0,
                zoom: 1.0,
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
                x: 0.0,
                y: 0.0,
                zoom: 1.0,
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
                x: 0.0,
                y: 0.0,
                zoom: 1.0,
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
