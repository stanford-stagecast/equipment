<script lang="ts">
  import ActorTab from './ActorTab.svelte';
  import type { Actor } from './types';
  export let actors: Actor = [
    {
      name: "Kirk",
      url: "big-buck-bunny.mp4",
      outputs: [
        {
          name: "Close Up",
          current: {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0,
          },
          presets: [
            {
              name: "Full",
              config: {
                top: 0,
                right: 0,
                bottom: 0,
                left: 0,
              },
            },
            {
              name: "Small",
              config: {
                top: 0,
                right: 100,
                bottom: 100,
                left: 0,
              },
            }
          ],
        },
        {
          name: "Wide",
          current: {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0,
          },
          presets: [
            {
              name: "Full",
              config: {
                top: 0,
                right: 0,
                bottom: 0,
                left: 0,
              },
            },
            {
              name: "Small",
              config: {
                top: 0,
                right: 100,
                bottom: 100,
                left: 0,
              },
            }
          ],
        },
      ]
    }, {
      name: "Spock",
      url: "elephants-dream.mp4",
      outputs: [
        {
          name: "Close Up",
          current: {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0,
          },
          presets: [],
        }
      ]
    }
  ];
  export let current = 0;

  function selectTab(i: number) {
    current = i;
  }
</script>

<main>
  <div id="tab_bar">
    {#each actors as actor, i}
      {#if i === current}
        <button class="tab current" disabled>{actor.name}</button>
      {:else}
        <button class="tab" on:click={() => selectTab(i)}>{actor.name}</button>
      {/if}
    {/each}
  </div>
  {#each actors as actor, i}
    <div class="body" class:hidden={i !== current}>
      <ActorTab actor={actor}/>
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
    background-color: #5555ff;
  }

  .body {
    border: 1px solid black;
    margin: 10px;
  }

  .body.hidden {
    display: none;
  }

  @media (min-width: 640px) {
    main {
      max-width: none;
    }
  }
</style>
