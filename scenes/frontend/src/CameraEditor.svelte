<script lang="ts">
  import type { State, Camera } from './types';

  export let state: State;
  export let camera: Camera;
  export let sync: Function;

  function remove() {
    let yes = window.confirm(`Are you sure you want to delete the camera view "${camera.name}"?`)
    if (yes !== true) return;
    state.cameras = state.cameras.filter(x => x.id !== camera.id);
    state.cues.map(x => {
      x.changes = x.changes.filter(y => {
        return y.type !== "camera" || y.camera_id != camera.id;
      });
    });
    state.cues = state.cues;
    sync();
  }

</script>

<main>
  <input type="text" bind:value={camera.name} on:change={sync}/>
  <button on:click={remove}>Delete</button>
</main>

<style>
</style>
