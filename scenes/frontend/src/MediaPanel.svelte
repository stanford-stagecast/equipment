<script lang="ts">
  import type { State, Camera } from './types';
  import MediaEditor from './MediaEditor.svelte';
  export let state: State;
  export let sync: Function;

  let active = false;
  $: active = state.media && state.media.length > 0;

  function new_media() {
    let media_id = state.media.reduce((p, m) => m.id > p ? m.id : p, 0) + 1;
    state.media.push({
      name: "",
      file: "",
      id: media_id,
      type: "image",
    });
    state.media = state.media;
  }

</script>

<main>
  <h1>Media</h1>
  {#if active}
    {#each state.media as media (media.id)}
      <MediaEditor bind:state={state} media={media} sync={sync}/>
    {/each}
  {/if}
  <button on:click={new_media}>Add Media</button>
</main>

<style>
</style>
