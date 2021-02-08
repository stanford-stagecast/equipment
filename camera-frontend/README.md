# Camera Control Interface

## Building
- Install npm
- run `make run` to test the camera interface live
- run `make build` to "compile" the frontend to regular HTML/JS

## Server messages

By default, the code tries to connect to a secure websocket server at port 5000
on the same server the website is hosted from. This can be changed in
`server.ts`, line 6.

Commenting out line 3 and uncommenting line 4 of `App.svelte` will tell the
frontend to use a simulated server instead of establishing a real websocket
connection.  The file paths of the "streams" served by this fake server can be
edited in `fake_server.ts`.

The server needs to be able to handle the following messages:
- `add-view`
- `update-view`
- `delete-view`

Additionally, the server needs to send the `sync-state` message to the client
every time the state on the backend changes.

Exact message formats are described in `message.ts`, and the data payload
formats are described in `types.ts`.

