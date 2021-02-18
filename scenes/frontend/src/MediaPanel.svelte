<script lang="ts">
  import type { State } from './types';
import MediaViewer from './MediaViewer.svelte';

  export let state: State;

  let active = false;
  $: active = state.media !== undefined;

</script>

<main>
  <h1>Media</h1>
  {#if active}
    <ul>
      {#each state.media as media}
        <MediaViewer media={media}/>
      {/each}
    </ul>
  {/if}
</main>

<style>
  main {
    margin: 10px;
  }
</style>
