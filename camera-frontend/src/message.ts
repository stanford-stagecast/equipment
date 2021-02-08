import type {State, View} from './types';

type ServerToClient = {type: 'sync-state', data: State};

type AddView = {type: 'add-view', data: {name: string}};
type UpdateView = {type: 'update-view', data: View};
type DeleteView = {type: 'delete-view', data: {id: number}};

type ClientToServer = AddView | UpdateView | DeleteView;

export type Message = ServerToClient | ClientToServer;
