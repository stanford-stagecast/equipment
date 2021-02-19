<script lang="ts">
  import type { State, Cue, CameraChange, MediaChange, LinkedCueChange } from './types';
  import ChangeEditor from './ChangeEditor.svelte';

  export let state: State;
  export let cue: Cue;
  export let sync: Function;

  let playing: Set<number>;
  let visible: Set<number>;
  let playing_after: Set<number>;
  let visible_after: Set<number>;
  let z_indexes = {
    media: {},
    cameras: {}
  };
  let lights: number;
  let sound: number;

  let current_change = 0;
  $: {
    if (current_change >= cue.changes.length) current_change = 0;
  }

  $: {
    visible = new Set();
    playing = new Set();
    visible_after = new Set();
    playing_after = new Set();
    z_indexes = {
      media: {},
      cameras: {}
    };
    lights = 0;
    sound = 0;

    let current = 0;
    while (current < state.cues.length && state.cues[current].number <= cue.number) {
      if (state.cues[current].number === cue.number) {
        visible = new Set([...visible_after]);
        playing = new Set([...playing_after]);
      }
      for (let change of state.cues[current].changes) {
        if (change.type === "media") {
          if (change.action.type === "add") {
            playing_after.add(change.media_id);
            z_indexes.media[change.media_id] = change.action.config.view.z;
          } else if (change.action.type === "remove") {
            playing_after.delete(change.media_id);
          } else if (change.action.type === "update") {
            z_indexes.media[change.media_id] = change.action.config.view.z;
          }
        } else if (change.type === "linked") {
          if (change.cue_type === "light") {
            lights = change.cue_number;
          } else if (change.cue_type === "sound") {
            sound = change.cue_number;
          }
        } else if (change.type === "camera") {
          if (change.action.type === "add") {
            visible_after.add(change.camera_id);
            z_indexes.cameras[change.camera_id] = change.action.config.view.z;
          } else if (change.action.type === "remove") {
            visible_after.delete(change.camera_id);
          } else if (change.action.type === "update") {
            z_indexes.cameras[change.camera_id] = change.action.config.view.z;
          }
        }
      }
      current += 1;
    }
  }

  function get_media_name(change: MediaChange) {
    let action = change.action.type;
    let actionName = action[0].toUpperCase() + action.slice(1);
    let media = state.media.find(x => x.id == change.media_id);
    if (media === undefined) return "INVALID";
    let name = media.name;
    let add = "";
    if (action !== "add" && !playing.has(change.media_id)) {
      add = " (ERROR)"
    }
    return `${actionName} Media <${name}> (${change.time}s)${add}`;
  }

  function get_camera_name(change: CameraChange) {
    let action = change.action.type;
    let actionName = action[0].toUpperCase() + action.slice(1);
    let camera = state.cameras.find(x => x.id == change.camera_id);
    if (camera === undefined) return "INVALID";
    let name = camera.name;
    let add = "";
    if (action !== "add" && !visible.has(change.camera_id)) {
      add = " (ERROR)"
    }
    return `${actionName} Camera <${name}> (${change.time}s)${add}`;
  }

  function get_linked_name(change: LinkedCueChange) {
    let type = change.cue_type;
    let list = type[0].toUpperCase() + type.slice(1);
    return `${list} Cue ${change.cue_number / 100} Go`;
  }

  function get_camera_names(visible) {
    return [...visible].map(x => {
      return state.cameras.find(y => y.id == x).name;
    }).join(", ");
  }

  function get_media_names(playing) {
    return [...playing].map(x => {
      return state.media.find(y => y.id == x).name;
    }).join(", ");
  }

  function get_stack(visible_after, playing_after) {
    let cameras = [...visible_after]
      .map(x => state.cameras.find(y => y.id === x))
      .filter(x => x)
      .map(x => {
        return {name: x.name, z: z_indexes.cameras[x.id], type: "📷"};
      });
    let media = [...playing_after]
      .map(x => state.media.find(y => y.id === x))
      .filter(x => x.type !== "audio")
      .filter(x => x)
      .map(x => {
        return {
          name: x.name,
          z: z_indexes.media[x.id],
          type: {
            'video': '🎥',
            'audio': '🎧',
            'image': '🖼️',
          }[x.type]
        };
      });
    let stack = [...cameras, ...media].sort((a, b) => {
      return a.z - b.z;
    });
    return stack;
  }

  function add_action() {
    let new_change: LinkedCueChange = {
      type: "linked",
      cue_type: "light",
      cue_number: 0,
    }
    cue.changes.push(new_change);
    cue.changes = cue.changes;
    sync();
  }

  function remove_action(action) {
    cue.changes = cue.changes.filter(x => x !== action);
    sync();
  }

</script>

<main>
  <h2>Cue #{cue.number/100}<br/></h2>
  Name: <input type="text" bind:value={cue.name} on:change={sync}/><br/>
  <strong>Live Cameras:</strong> {get_camera_names(visible) || "N/A"}<br/>
  <strong>Live Media:</strong> {get_media_names(playing) || "N/A"}<br/>
  <strong>Lights:</strong> {lights / 100}<br/>
  <strong>Sound:</strong> {sound / 100}<br/>
  <div class="row">
    <div id="change">
      <h3>Actions in this Cue</h3>
      {#each cue.changes as change, i}
        {#if change.type == "media"}
          <input type="radio" id={i.toString()} name="change" value={i}
                                                bind:group={current_change}>
          <label for={i.toString()}>{get_media_name(change)}</label><br/>
        {:else if change.type == "camera"}
          <input type="radio" id={i.toString()} name="change" value={i}
                                                bind:group={current_change}>
          <label for={i.toString()}>{get_camera_name(change)}</label><br/>
        {:else if change.type == "linked"}
          <input type="radio" id={i.toString()} name="change" value={i}
                                                bind:group={current_change}>
          <label for={i.toString()}>{get_linked_name(change)}</label><br/>
        {/if}
      {/each}
      <br/>
      <button on:click={add_action}>Add Action</button>
    </div>
    <div id="action">
      {#if cue.changes[current_change]}
        <ChangeEditor bind:state={state} bind:change={cue.changes[current_change]} sync={sync}
                      visible={visible} playing={playing}
                      remove_change={remove_action}/>
      {/if}
    </div>
    <div id="stack">
      <h3>Output Order</h3>
      [Front]<br/>
      <ol>
      {#each get_stack(visible_after, playing_after) as item}
        <li>{item.type}: {item.name} ({item.z})</li>
      {/each}
      </ol>
      [Back]<br/>
    </div>
  </div>
</main>

<style>
  input, label {
    display: inline-block;
  }

  #change {
    width: 100%;
    border: 1px solid black;
    padding: 10px;
  }

  input[type="radio"] {
    margin-top: 10px;
  }

  .row {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
  }

  #action {
    border: 1px solid black;
    padding: 10px;
    width: 100%;
    margin-left: 10px;
  }

  #stack {
    border: 1px solid black;
    padding: 10px;
    width: 100%;
    margin-left: 10px;
  }

  #stack li {
    font-size: 16pt;
  }

</style>
