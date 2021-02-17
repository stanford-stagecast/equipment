<script lang="ts">
  import { onMount } from 'svelte';
  import server from './server';
  import type { State, Cue } from './types';
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
        state.cues = sorted(state.cues);
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
      if ((current === undefined || current === "") && state.cues && state.cues.length > 0) {
        current = state.cues[0].number.toString();
      }
    });
  });

  let active = false;
  $: {
    active = state.cues !== undefined && state.cues.length !== 0 && current !== undefined;
  }

  function sorted(cues: Cue[]) {
    return cues.sort((a, b) => {
      if (a.number < b.number) {
        return -1;
      } else if (a.number > b.number) {
        return 1;
      } else {
        return 0;
      }
    });
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
    state.cues = sorted(state.cues);
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

  function copy() {
    let name = window.prompt(`Enter a number for the new copy of cue #${parseInt(current)/100} (up to 2 decimal places)`);
    if (name === undefined || name === "") return;
    let num = parseFloat(name);
    if (isNaN(num)) {
      window.alert(`Invalid cue number: ${num}`);
      return;
    }
    let adjusted = parseInt((num * 100).toFixed(0));
    if (adjusted / 100 !== num) {
      window.alert(`Invalid cue number: ${num}`);
      return;
    }
    let exists = state.cues.reduce((prev, curr, i) => {
      return prev || curr.number === adjusted;
    }, false);
    if (exists) {
      let yes = window.confirm(`Cue ${num} already exists! Are you sure you want to overwrite it?`)
      if (yes !== true) return;
    }
    let src = state.cues.find((x) => {
      return x.number.toString() === current;
    });
    state.cues = state.cues.filter((x) => {
      return x.number !== adjusted;
    });
    state.cues.push({
      ...src,
      deltas: JSON.parse(JSON.stringify(src.deltas)),
      number: adjusted,
    });
    state.cues = sorted(state.cues);
    console.log(state.cues);
    server.sendMessage({
      'type': 'update',
      'data': state,
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
    <button id="copy_cue" on:click={copy} disabled={!active}>Copy Cue</button>
    <br/>
  </div>
  <div id="rows">
    {#if active}
      <table id="cues">
        <thead>
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
        </thead>
        <tbody>
          {#each state.cues as cue (cue)}
            <CueRow channels={state.channels} cue={cue} sync={sync}/>
          {/each}
        </tbody>
      </table>
    {/if}
  </div>
</main>

<style>
  table {
    border-collapse: separate;
    border-spacing: 0;
    border-top: 1px solid black;
    border-left: 1px solid black;
    margin: 10px;
  }

  th {
    padding: 10px;
    border-bottom: 1px solid black;
    border-right: 1px solid black;
    top: 0;
    position: sticky;
    background-color: white;
    background-clip: padding-box;
    margin: 0;
  }

  @media (min-width: 640px) {
    main {
      max-width: none;
    }
  }
</style>
