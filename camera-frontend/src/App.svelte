<script lang="ts">
  import { onMount } from 'svelte';
  import server from './server';
  /* import server from './fake_server'; */
  import type { State, View } from './types';

  import ViewControl from './ViewControl.svelte';

  let current: string;

  let state: State = {
    views: [],
    inputs: [],
  };

  let view: View;
  $: {
    if (state.views) {
      state.views.map(x => {
        if (x.id.toString() === current) {
          view = x;
        }
      });
    }
  }

  onMount(() => {
    server.subscribe(msg => {
      state = msg;
      if ((current === undefined || current === "") && state.views && state.views !== []) {
        current = state.views[0].id.toString();
      }
    });
  });

  let active = false;
  $: {
    active = state.views !== undefined && state.views.length !== 0;
  }

  function add_view() {
    let name = window.prompt("Enter a name for the new view");
    if (name === undefined) return;
    server.sendMessage({
      type: 'add-view',
      data: {name},
    });
  }

</script>

<main>
  <div id="view_picker">
    Currently Editing:
    <select id="select_view" disabled={!active}
            bind:value={current}>
      {#if active}
        {#each state.views as view}
          <option value={view.id.toString()}>{view.name}</option>
        {/each}
      {:else}
        <option value="">N/A</option>
      {/if}
    </select>
    <button id="add_view" on:click={add_view}>New View</button>
    <br/>
    {#if active}
      <ViewControl view={view} inputs={state.inputs}
        sendMessage={server.sendMessage}/>
    {/if}
  </div>
</main>

<style>

  @media (min-width: 640px) {
    main {
      max-width: none;
    }
  }
</style>
