import type {State} from './types';

type Sync = {type: 'sync', data: State};

type RequestUpdate = {type: 'update', data: State};
type RequestSync = {type: 'sync'};
type RequestSave = {type: 'save'};

type ClientToServer = RequestUpdate | RequestSync | RequestSave;

export type Message = Sync | ClientToServer;
