# Frontend Documentation

The messages the server needs to handle are in `./message.ts`, and the JSON format is documented at `./types.ts`.

Weird behavioral things: the frontend assumes that a given media file is only played once at a time.  If a media file is added while it's already playing, the second "add media" action should be treated as a "change media" action.
