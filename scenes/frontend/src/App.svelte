<script lang="ts">
  import { onMount } from 'svelte';
  /* import server from './fake_server'; */
  import server from './server';
  import type { State } from './types';
  import CuePanel from './CuePanel.svelte';
  import CameraPanel from './CameraPanel.svelte';
  import MediaPanel from './MediaPanel.svelte';

  let state: State = {
    cameras: [],
    media: [],
    cues: [],
  };

  let initialized = false;

  onMount(() => {
    server.subscribe((msg: State) => {
      state = msg;
      initialized = true;
    });
  });

  function sync() {
    server.sendMessage({
      'type': 'update',
      'data': state,
    });
  }

  function save() {
    server.sendMessage({
      'type': 'save',
    });
  }
</script>

<main>
  <div id="global">
    <button id="save" on:click={save} disabled={!initialized}>Save to Disk</button>
    <br/>
  </div>
  <div id="cues" class="panel">
    <CuePanel sync={sync} bind:state={state}/>
  </div>
  <div id="media" class="panel">
    <MediaPanel sync={sync} bind:state={state}/>
  </div>
  <div id="cameras" class="panel">
    <CameraPanel sync={sync} bind:state={state}/>
  </div>
</main>

<style>
  .panel {
    border: 1px solid black;
    margin-bottom: 25px;
    padding: 10px;
  }

  @media (min-width: 640px) {
    main {
      max-width: none;
    }
  }
</style>
