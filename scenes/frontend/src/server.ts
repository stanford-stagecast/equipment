import { writable } from 'svelte/store';
import type { Message } from './message';

const wsStore = writable({});

const socket = new WebSocket(`ws://${window.location.hostname}:8001/socket`);

socket.addEventListener('open', () => {
  socket.send(JSON.stringify({'type': 'sync'}));
});

socket.addEventListener('message', (event) => {
  let json = JSON.parse(event.data);
  let type = json.type;
  switch (type) {
    case 'sync': {
      let data = json.data;
      wsStore.set(data);
      break;
    }
    case 'success': {
      window.alert(json.description);
      break;
    }
    case 'error': {
      window.alert(json.description);
      break;
    }
    default: {
      console.error("Unknown message:", event.data);
      break;
    }
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
