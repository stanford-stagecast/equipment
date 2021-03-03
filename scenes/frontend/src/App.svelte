<script lang="ts">
  import { onMount } from 'svelte';
  /* import server from './fake_server'; */
  import server from './server';
  import type { State } from './types';
  import CuePanel from './CuePanel.svelte';
  import CameraPanel from './CameraPanel.svelte';
  import MediaPanel from './MediaPanel.svelte';
  import LivePanel from './LivePanel.svelte';

  let state: State = {
    cameras: [],
    media: [],
    cues: [],
  };
  let playing_cue = 0;

  let initialized = false;

  onMount(() => {
    server.subscribe((msg: State) => {
      state = msg;
      initialized = true;
    });
    server.subscribe_cue((cue: number) => {
      playing_cue = cue;
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

  function go(q: number) {
    server.sendMessage({
      'type': 'go',
      'data': {
        'cue': q,
      }
    });
  }
</script>

<main>
  <div id="global">
    <button id="save" on:click={save} disabled={!initialized}>Save to Disk</button>
    <br/>
  </div>
  <div id="live" class="panel">
    <LivePanel go={go} bind:live={playing_cue} bind:state={state}/>
  </div>
  <div id="cues" class="panel">
    <CuePanel sync={sync} go={go} bind:state={state}/>
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
