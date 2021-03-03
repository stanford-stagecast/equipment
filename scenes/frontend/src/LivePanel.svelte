<script lang="ts">
  import type {State, Cue} from './types';
  import CueEditor from './CueEditor.svelte';

  export let state: State;
  export let live: number;
  export let go: Function;

  let active = false;
  let next = undefined;
  let next_name = "";

  $: {
    next = undefined;
    next_name = "";
    if (state.cues) {
      let index = state.cues.findIndex((value) =>
        value.number == live
      );
      index++;
      if (index < state.cues.length) {
        let cue = state.cues[index];
        next = cue.number;
        next_name = cue.name;
      }
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
    go(next);
  }
</script>

<main>
    <h1>Live</h1>
    Current Cue: {(live / 100).toString()}
    <br/>
    Next Cue: {(next / 100).toString()} ("{next_name}")
    <br/>
    <br/>
    <button id="next" on:click={next_cue} disabled={next === undefined}>Next Cue GO</button>
    <br/>
</main>

<style>
  * {
    font-size: 24pt;
  }
</style>
