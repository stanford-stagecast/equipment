import { writable } from 'svelte/store';
import type { Message } from './message';

// @ts-ignore
window.current_state = {
  views: [
    {
      id: 0,
      name: "Close-up",
      input_id: 0,
      config: {
        top: 0,
        bottom: 0,
        right: 0,
        left: 0,
        color_temperature: 5000, // Color Temperature in Kelvin; ~1700-6500
        compositing_color_similarity: 500, // Green screening color similarity (0-1000)
      },
      presets: [
        {
          name: "Default",
          config: {
            top: 0,
            bottom: 0,
            right: 0,
            left: 0,
            color_temperature: 5000,
            compositing_color_similarity: 500,
          },
        },
        {
          name: "Small",
          config: {
            top: 100,
            bottom: 100,
            right: 100,
            left: 100,
            color_temperature: 3600,
            compositing_color_similarity: 900,
          },
        }
      ]
    }
  ],
  inputs: [
    {
      id: 0,
      name: "Big Buck Bunny",
      url: "big-buck-bunny.mp4",
    },
    {
      id: 1,
      name: "Elephants Dream",
      url: "elephants-dream.mp4",
    }
  ],
};
// @ts-ignore
window.i = 10;

// @ts-ignore
const wsStore = writable(window.current_state);

const sendMessage = (message: Message) => {
  console.log(message);
  switch (message.type) {
    case 'add-view': {
      // @ts-ignore
      window.current_state.views.push({
        name: message.data.name,
        // @ts-ignore
        id: window.i++,
        input_id: 0,
        config: {
          top: 0,
          bottom: 0,
          left: 0,
          right: 0,
          color_temperature: 5000,
          compositing_color_similarity: 500,
        },
        presets: [],
      });
      break;
    }
    case 'update-view': {
      // @ts-ignore
      window.current_state.views = window.current_state.views.map((x) => {
        if (x.id === message.data.id) {
          Object.assign(x, message.data);
        }
        return x;
      });
      break;
    }
    case 'delete-view': {
      // @ts-ignore
      window.current_state.views = window.current_state.views.filter((x) => {
        if (x.id === message.data.id) {
          return false;
        }
        return true;
      });
      break;
    }
  }
  // @ts-ignore
  wsStore.set(window.current_state);
}

export default {
  subscribe: wsStore.subscribe,
  sendMessage,
}
