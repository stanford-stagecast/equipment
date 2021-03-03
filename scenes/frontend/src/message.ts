import type {State} from './types';

type Sync = {type: 'sync', data: State};
type Err = {type: 'error', description: string};
type Success = {type: 'success', description: string};
type Go = {type: 'go', data: {cue: number, jump: boolean}};

type RequestGo = {type: 'go', data: {cue: number}};
type RequestUpdate = {type: 'update', data: State};
type RequestSync = {type: 'sync'};
type RequestSave = {type: 'save'};

type ServerToClient = Sync | Err | Success | Go;
type ClientToServer = RequestUpdate | RequestSync | RequestSave | RequestGo;

export type Message = ServerToClient | ClientToServer;
