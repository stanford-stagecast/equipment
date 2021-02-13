<script lang="ts">
  import { onMount } from 'svelte';
  import server from './server';
  import type { State } from './types';
  import CueRow from './CueRow.svelte';

  let current: string;

  let state: State = {
    channels: [],
    cues: [],
  };

  onMount(() => {
    server.subscribe((msg: State) => {
      state = msg;
      if (state.cues !== undefined) {
        state.cues = state.cues.sort((a, b) => {
          if (a.number < b.number) {
            return -1;
          } else if (a.number > b.number) {
            return 1;
          } else {
            return 0;
          }
        });
      }
      if (state.channels !== undefined) {
        state.channels = state.channels.sort((a, b) => {
          if (a.number < b.number) {
            return -1;
          } else if (a.number > b.number) {
            return 1;
          } else {
            return 0;
          }
        });
      }
      if ((current === undefined || current === "") && state.channels && state.channels !== []) {
        current = state.cues[0].number.toString();
      }
    });
  });

  let active = false;
  $: {
    active = state.cues !== undefined && state.cues.length !== 0 && current !== undefined;
  }

  function add_cue() {
    let name = window.prompt("Enter a number for the new cue (up to 2 decimal places)");
    if (name === undefined || name === "") return;
    let num = parseFloat(name);
    if (isNaN(num)) {
      window.alert(`Invalid cue number: ${num}`);
      return;
    }
    let adjusted = Math.floor(num * 100);
    if (adjusted / 100 !== num) {
      window.alert(`Invalid cue number: ${num}`);
      return;
    }
    let exists = state.cues.reduce((prev, curr, i) => {
      return prev || curr.number === adjusted;
    }, false);
    if (exists) {
      window.alert(`Cue ${num} already exists.`);
      return;
    }
    state.cues.push({
      number: adjusted,
      name: "",
      time: 1,
      deltas: [],
    });
    state.cues = state.cues.sort((a, b) => {
      if (a.number < b.number) {
        return -1;
      } else if (a.number > b.number) {
        return 1;
      } else {
        return 0;
      }
    });
    server.sendMessage({
      'type': 'update',
      'data': state,
    });
    current = adjusted.toString();
  }

  function delete_cue() {
    let yes = window.confirm(`Are you sure you want to delete cue #${parseInt(current)/100}?`);
    if (yes !== true) return;
    state.cues = state.cues.filter((x) => {
      return x.number.toString() !== current;
    });
    current = undefined;
    server.sendMessage({
      'type': 'update',
      'data': state,
    });
  }

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
  <div id="cue_picker">
    <button id="add_cue" on:click={add_cue}>New Cue</button>
    <button id="save" on:click={save} disabled={!active}>Save to Disk</button>
    <br/>
    <select id="select_cue" disabled={!active}
            bind:value={current}>
      {#if active}
        {#each state.cues as cue}
          <option value={cue.number.toString()}>
            {cue.number / 100}
            {cue.name ?  ": " : ""}
            {cue.name}
          </option>
        {/each}
      {:else}
        <option value="">N/A</option>
      {/if}
    </select>
    <button id="delete_cue" on:click={delete_cue} disabled={!active}>Delete Cue</button>
    <br/>
  </div>
  <div id="rows">
    {#if active}
      <table id="cues">
        <tr>
          <th>Number</th>
          <th>Name</th>
          <th>Fade (s)</th>
          {#each state.channels as channel}
            <th>
              {channel.number}
              <br/>
              {channel.name}
            </th>
          {/each}
        </tr>
        {#each state.cues as cue}
          <CueRow channels={state.channels} cue={cue} sync={sync}/>
        {/each}
      </table>
    {/if}
  </div>
</main>

<style>
  table, th {
    border: 1px solid black;
    border-collapse: collapse;
  }

  @media (min-width: 640px) {
    main {
      max-width: none;
    }
  }
</style>
