<script lang="ts">
  import ViewControl from './ViewControl.svelte';
  import { onMount } from 'svelte';
  import type { Output } from './types';

  export let name: string;
  export let url: string;
  export let output: Output;

  let video: HTMLVideoElement;

  let start: (video: HTMLVideoElement) => void;
  onMount(() => {
    start(video);
  });

</script>

<main>
  <h1>Viewing {name} - {output.name}</h1>
  <video bind:this={video} muted autoplay width="340" height="240">
    <source src={url} type="video/mp4"/>
  </video>
  <ViewControl output={output} bind:start={start}/>
</main>

<style>
  main {
    text-align: center;
    padding: 1em;
    margin: 0 auto;
  }

  video {
    vertical-align: top;
  }

  h1 {
    color: #ff3e00;
    text-transform: uppercase;
    font-size: 2em;
    font-weight: 400;
  }
</style>
