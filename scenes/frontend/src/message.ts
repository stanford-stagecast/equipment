import type {State} from './types';

type Sync = {type: 'sync', data: State};
type Err = {type: 'error', description: string};
type Success = {type: 'success', description: string};

type RequestUpdate = {type: 'update', data: State};
type RequestSync = {type: 'sync'};
type RequestSave = {type: 'save'};

type ServerToClient = Sync | Err | Success;
type ClientToServer = RequestUpdate | RequestSync | RequestSave;

export type Message = ServerToClient | ClientToServer;
