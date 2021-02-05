<script lang="ts">
  import ViewTab from './ViewTab.svelte';
  import type {Actor} from './types';

  export let actor: Actor;

  let current = 0;
  function selectTab(i: number) {
    current = i;
  }
</script>

<main>
  <div id="tab_bar">
    {#each actor.outputs as output, i}
      {#if i === current}
        <button class="tab current" disabled>{output.name}</button>
      {:else}
        <button class="tab" on:click={() => selectTab(i)}>{output.name}</button>
      {/if}
    {/each}
  </div>
  {#each actor.outputs as output, i}
    <div class="body" class:hidden={i !== current}>
      <ViewTab name={actor.name} url={actor.url} output={output}/>
    </div>
  {/each}
</main>

<style>
  .tab {
    padding: 10px;
    margin: 5px;
  }

  .tab.current {
    color: white;
    background-color: #559955;
  }

  .body {
    border: 1px solid black;
    margin: 10px;
  }

  .body.hidden {
    display: none;
  }
</style>
