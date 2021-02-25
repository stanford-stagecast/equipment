<script lang="ts">
  import type { State, Change } from './types';
  import { WIDTH, HEIGHT } from './types';
  export let state: State;
  export let change: Change;
  export let sync: Function;
  export let visible: Set<number>;
  export let playing: Set<number>;
  export let remove_change: Function;

  let cue_number = undefined;
  $: {
    if (change.type === "linked" && cue_number == undefined) {
      cue_number = change.cue_number / 100;
    } else if (change.type !== "linked") {
      cue_number = undefined;
    }
  }

  function update_number() {
    if (cue_number !== undefined) {
      change.cue_number = parseInt((cue_number * 100).toFixed(0));
      cue_number = change.cue_number / 100;
    }
  }

  function update_action() {
    if (change.type === "linked") {
      change.cue_number = change.cue_number || 0;
      change.cue_type = change.cue_type || "light";
      change.action = undefined;
      change.time = undefined;
      return;
    }

    change.time = change.time || 0.0;

    if (change.action === undefined) {
      change.action = {
        type: "add",
      }
    }
    if (change.action.type === "remove") {
      change.action.config = undefined;
    } else {
      let blank_view = {
        zoom: 1.0,
        top: 0.0,
        left: 0.0,
        right: WIDTH,
        bottom: HEIGHT,
      };
      let view = Object.assign(blank_view, change.action.config?.view);
      let blank_playback = {
        volume: 1.0,
        playing: true,
        done: "stop",
      }
      let playback = Object.assign(blank_playback, change.action.config?.playback);
      if (change.type === "camera") {
        change.camera_id = change.camera_id || state.cameras[0].id;
        change.action.config =  {
          view
        };
      } else {
        change.media_id = change.media_id || state.media[0].id;
        if (media_type(change.media_id) !== "video" && playback.done ===
        "hold") {
          playback.done = "stop";
        }
        change.action.config =  {
          view, playback
        };
      }
    }
  }

  function update_not_null() {
    if (change?.action?.config?.view) {
      change.action.config.view.x = change.action.config.view.x || 0.0;
      change.action.config.view.y = change.action.config.view.y || 0.0;
      change.action.config.view.zoom = change.action.config.view.zoom || 1.0;
      change.action.config.view.z = change.action.config.view.z || 0;
    }
    if (change?.action?.config?.playback) {
      change.action.config.playback.volume = change.action.config.playback.volume || 0.0;
    }
  }

  function update() {
    update_action();
    update_number();
    update_not_null();
    sync();
  }

  function media_type(id) {
    return state.media.find(x => x.id === id)?.type;
  }

  function needs_config(change) {
    return change.type !== "linked" && change.action.type !== "remove";
  }

  function needs_view(change) {
    return change.type === "camera" || media_type(change.media_id) !== "audio";
  }

  function needs_playback(change) {
    return change.type === "media" && media_type(change.media_id) !== "image";
  }

  function delete_change() {
    let yes = window.confirm("Are you sure you want to delete this action?");
    if (yes !== true) return;
    remove_change(change);
  }
</script>

<main>
  Action Type:<br/>
  <!-- svelte-ignore a11y-no-onchange -->
  <select bind:value={change.type} on:change={update}>
    {#if state.media && state.media.length > 0}
      <option value="media">Media</option>
    {/if}
    {#if state.cameras && state.cameras.length > 0}
      <option value="camera">Camera</option>
    {/if}
    <option value="linked">Linked Cue</option>
  </select><br/>

  {#if change.type == "linked"}
    Cue Type:<br/>
    <!-- svelte-ignore a11y-no-onchange -->
    <select bind:value={change.cue_type} on:change={update}>
      <option value="light">Light Cue</option>
      <option value="sound">Sound Cue</option>
    </select><br/>
    Cue Number:<br/>
    <input type="number" step="0.01" bind:value={cue_number}
                         on:change={update}/>
  {:else}
    Time:<br/>
    <input type="number" bind:value={change.time} on:change={update}><br/>
    Action:<br/>
    <!-- svelte-ignore a11y-no-onchange -->
    <select bind:value={change.action.type} on:change={update}>
      <option value="add">Add</option>
      <option value="update">Update</option>
      <option value="remove">Remove</option>
    </select><br/>
    {#if change.type == "media"}
      <!-- svelte-ignore a11y-no-onchange -->
      Media: <select bind:value={change.media_id} on:change={update}>
        {#each state.media as media}
          <option value={media.id}>{media.name}
          {#if playing.has(media.id)}
            (LIVE)
          {/if}
          </option>
        {/each}
      </select><br/>
    {:else if change.type == "camera"}
      Camera:<br/>
      <!-- svelte-ignore a11y-no-onchange -->
      <select bind:value={change.camera_id} on:change={update}>
        {#each state.cameras as camera}
          <option value={camera.id}>{camera.name}
          {#if visible.has(camera.id)}
            (LIVE)
          {/if}
          </option>
        {/each}
      </select><br/>
    {/if}

    {#if needs_config(change)}
      {#if needs_view(change)}
        <div id="view_config">
          Top:<br/>
          <input type="range" min="0" max={HEIGHT} bind:value={change.action.config.view.top} on:change={update}/><br/>
          Right:<br/>
          <input type="range" min="0" max={WIDTH} bind:value={change.action.config.view.right} on:change={update}/><br/>
          Bottom:<br/>
          <input type="range" min="0" max={HEIGHT} bind:value={change.action.config.view.bottom} on:change={update}/><br/>
          Left:<br/>
          <input type="range" min="0" max={WIDTH} bind:value={change.action.config.view.left} on:change={update}/><br/>
          Z (0=front, higher=further back):<br/>
          <input type="number" bind:value={change.action.config.view.z} on:change={update}/><br/>
        </div>
      {/if}
      {#if needs_playback(change)}
        <div id="playback_config">
          Volume (0=mute, 1=default):<br/>
          <input type="number"
                 bind:value={change.action.config.playback.volume} on:change={update}/><br/>
          Play: <input type="checkbox" bind:checked={change.action.config.playback.playing} on:change={update}/><br/>
        </div>
        When Done:<br/>
        <!-- svelte-ignore a11y-no-onchange -->
        <select bind:value={change.action.config.playback.done} on:change={update}>
          {#if media_type(change.media_id) == "video"}
            <option value="hold">Hold Last Frame</option>
          {/if}
          <option value="loop">Loop</option>
          <option value="stop">Stop Media</option>
        </select><br/>
      {/if}
    {/if}
  {/if}
  <button on:click={delete_change}>Delete</button>
</main>

<style>
</style>
