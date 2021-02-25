<script lang="ts">
  import type { State, Media } from './types';

  export let state: State;
  export let media: Media;
  export let sync: Function;

  function remove() {
    let yes = window.confirm(`Are you sure you want to delete the media "${media.name}"?`)
    if (yes !== true) return;
    state.media = state.media.filter(x => x.id !== media.id);
    state.cues.map(x => {
      x.changes = x.changes.filter(y => {
        return y.type !== "media" || y.media_id != media.id;
      });
    });
    state.cues = state.cues;
    sync();
  }
</script>

<main>
  Name: <input type="text" bind:value={media.name} on:change={sync}/>
  URL: <input type="text" bind:value={media.file} on:change={sync}/>
  <!-- svelte-ignore a11y-no-onchange -->
  Type: <select bind:value={media.type} on:change={sync}>
    <option value="image">Image</option>
    <option value="audio">Audio</option>
    <option value="video">Video</option>
  </select>
  <button on:click={remove}>Delete</button>
</main>

<style>
  input, select {
    margin-right: 20px;
  }
</style>
