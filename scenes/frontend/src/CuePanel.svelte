<script lang="ts">
  import type {State, Cue} from './types';
  import CueEditor from './CueEditor.svelte';

  export let state: State;
  export let sync: Function;

  let current: string;
  let selected_cue: Cue;

  let active = false;
  let have_next = false;
  let have_previous = false;

  $: {
    if ((current === "" || current === undefined) && state.cues && state.cues.length > 0) {
      current = state.cues[0].number.toString();
    };

    selected_cue = undefined;
    if (state.cues !== undefined && current !== undefined) {
      selected_cue = state.cues.find((q: Cue) => {
        return q.number.toString() == current;
      });
    }
    active = selected_cue !== undefined;
    if (selected_cue) {
      have_next = state.cues.reduce((prev, curr) => {
        return prev || curr.number > selected_cue.number;
      }, false);
      have_previous = state.cues.reduce((prev, curr) => {
        return prev || curr.number < selected_cue.number;
      }, false);
    } else {
      have_next = false;
      have_previous = false;
    }
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

  function next_cue() {
    let n = parseInt(current);
    let q = state.cues.reduce((prev, curr) => {
      if ((curr.number < prev.number) && (curr.number > n)) {
        return curr;
      }
      return prev;
    }, state.cues[state.cues.length - 1]);
    current = q.number.toString();
  }

  function previous_cue() {
    let n = parseInt(current);
    let q = state.cues.reduce((prev, curr) => {
      if ((curr.number > prev.number) && (curr.number < n)) {
        return curr;
      }
      return prev;
    }, state.cues[0]);
    current = q.number.toString();
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
      changes: [],
    });
    state.cues = sorted(state.cues);
    sync();
    current = adjusted.toString();
  }

  function remove_cue() {
    let yes = window.confirm("Are you sure you want to delete this cue?");
    if (yes !== true) return;
    state.cues = state.cues.filter(x => x.number.toString() !== current);
    current = undefined;
    sync();
  }
</script>

<main>
    <h1>Cues</h1>
    Viewing <select id="select_cue" disabled={!active} bind:value={current}>
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
    <button id="add" on:click={add_cue}>Add</button>
    <button id="delete" on:click={remove_cue} disabled={!active}>Delete</button>
    <br/>
    <button id="previous" on:click={previous_cue} disabled={!active || !have_previous}>Previous</button>
    <button id="next" on:click={next_cue} disabled={!active || !have_next}>Next</button>
    <br/>
    {#if active}
      <CueEditor bind:state={state} bind:cue={selected_cue} sync={sync}/>
    {/if}
</main>

<style>
</style>
