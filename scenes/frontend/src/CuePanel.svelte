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
    <button id="previous" on:click={previous_cue} disabled={!active || !have_previous}>Previous</button>
    <button id="next" on:click={next_cue} disabled={!active || !have_next}>Next</button>
    <br/>
    {#if active}
      <CueEditor bind:state={state} bind:cue={selected_cue} sync={sync}/>
    {/if}
</main>

<style>
</style>
