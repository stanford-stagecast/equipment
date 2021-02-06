import React, {Dispatch} from 'react';
import './ListControls.css';
import Server from '../../server';
import Action, {CueList} from '../../action';

export type ListData = {
  id: number,
  name: string,
  lists: CueList[]
};

type ListControlProps = {
  data: ListData;
  dispatch: Dispatch<Action>;
  server: Server;
}

export default function ListControls({data, dispatch, server}: ListControlProps) {
  let {id, name, lists} = data;
  let list_ids = lists.map(x => x.id);
  let free_id = 0;
  while (list_ids.indexOf(free_id) !== -1) {
    free_id++;
  }
  return (
    <div className="list_controls">
      Cue List ({id + 1}/{lists.length}):&nbsp;&nbsp;
      <select value={id} onChange={(e) => change_list(parseInt(e.target.value), dispatch)}>
        {
          lists.map(x => {
            return <option key={x.id} value={x.id}>{x.name}</option>
          })
        }
        <option value={free_id}>Add new...</option>
      </select>
      <button onClick={() => rename_list(id, name, server)}>Rename</button>
      <button onClick={() => remove_list(id, name, list_ids, server, dispatch)}>Delete</button>
    </div>
  );
}

function change_list(list_id: number, dispatch: Dispatch<Action>) {
  console.log(list_id);
  dispatch({type: 'switch_list', id: list_id});
}

function rename_list(list_id: number, old_name: string, server: Server) {
  let name = prompt(`Please enter a new name for this cue list (current is "${old_name}")`)
  if (name !== null) {
    server.rename_list(list_id, name);
  }
}

function remove_list(list_id: number, name: string, list_ids: number[], server: Server, dispatch: Dispatch<Action>) {
  let confirmation = window.confirm(`Are you sure you want to delete all the cues in "${name}"? This cannot be undone.`);
  if (confirmation === true) {
    server.delete_list(list_id);
    dispatch({type: 'switch_list', id: 0});
  }
}
