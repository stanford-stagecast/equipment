<script lang="ts">
  import type { State, Camera } from './types';
  import CameraEditor from './CameraEditor.svelte';
  export let state: State;
  export let sync: Function;

  let active = false;
  $: active = state.cameras && state.cameras.length > 0;

  function new_camera() {
    let camera_id = state.cameras.reduce((p, c) => c.id > p ? c.id : p, 0) + 1;
    state.cameras.push({
      name: "",
      id: camera_id,
    });
    state.cameras = state.cameras;
  }

</script>

<main>
  <h1>Cameras</h1>
  {#if active}
    {#each state.cameras as camera (camera.id)}
      <CameraEditor bind:state={state} camera={camera} sync={sync}/>
    {/each}
  {/if}
  <button on:click={new_camera}>Add Camera View</button>
</main>

<style>
</style>
