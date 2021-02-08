import { writable } from 'svelte/store';
import type { Message } from './message';

const wsStore = writable({});

const socket = new WebSocket(`wss://${window.location.hostname}:5000`);

socket.addEventListener('message', (event) => {
  let type = event.data.type;
  let data = event.data.data;
  if (type === 'sync-state') {
    wsStore.set(JSON.parse(data));
  } else {
    console.error("Unknown message:", event.data);
  }
});

const sendMessage = (message: Message) => {
  if (socket.readyState <= 1) {
    socket.send(JSON.stringify(message));
  }
}

export default {
  subscribe: wsStore.subscribe,
  sendMessage,
}
