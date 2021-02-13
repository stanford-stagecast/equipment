<script lang="ts">
  import type { State, Cue } from './types';

  export let sync: Function;
  export let channels: Channel[];
  export let cue: Cue;

  function map_channels() {
    return channels.map(chan => {
      let deltas = cue.deltas.filter(x => x.channel === chan.number);
      let delta = deltas.length > 0 ? deltas[0] : undefined;
      if (delta !== undefined) {
        delta = Object.assign({}, delta);
        delta.intensity = Math.round(delta.intensity * 100/255);
        delta.hue = Math.round(delta.hue * 100/255);
      }
      return {
        channel: chan,
        delta: delta,
      }
    });
  }
  let mappedChannels = map_channels();

  function remove_channel(channel) {
    cue.deltas = cue.deltas.filter(x => x.channel !== channel.number);
    mappedChannels = map_channels();
    sync();
  }

  function add_channel(channel) {
    cue.deltas = cue.deltas.filter(x => x.channel !== channel.number);
    cue.deltas = [...cue.deltas, {
      channel: channel.number,
      intensity: 0,
      hue: 0,
    }];
    mappedChannels = map_channels();
    sync();
  }

  function upload() {
    cue.deltas = mappedChannels
      .map(x => x.delta)
      .filter(x => x !== undefined)
      .map(x => {
        return {
          hue: Math.round(x.hue * 255/100),
          intensity: Math.round(x.intensity * 255/100),
          channel: x.channel
        }
      });
    sync();
    mappedChannels = map_channels();
  }

</script>

<tr>
  <td>
    {cue.number / 100}
  </td>
  <td>
    <input id="name" bind:value={cue.name} on:change={sync}/>
  </td>
  <td>
    <input id="time" type="number" bind:value={cue.time} on:change={sync}/>
  </td>
  {#each mappedChannels as mapped}
    <td>
      {#if mapped.delta}
        💡: <input type="number" min=0 max=100
                          bind:value={mapped.delta.intensity} on:change={upload}/>
        <br/>
        🎨: <input type="number" min=0 max=100 bind:value={mapped.delta.hue}
                    on:change={upload}/>
        <br/>
        <button on:click={() => remove_channel(mapped.channel)}>
          Remove
        </button>
      {:else}
        <button on:click={() => add_channel(mapped.channel)}>
          +
        </button>
      {/if}
    </td>
  {/each}
</tr>

<style>
  td {
    border: 1px solid grey;
    border-collapse: collapse;
    text-align: center;
    vertical-align: middle;
  }

  input#name {
    width: 100px;
  }

  input[type=number] {
    width: 75px;
  }
</style>
