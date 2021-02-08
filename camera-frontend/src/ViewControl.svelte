<script lang="ts">
  import type { View, Input, Preset } from './types';
  import { onMount, onDestroy } from 'svelte';

  export let view: View;
  export let inputs: Input[];
  export let sendMessage: Function;

  let input: Input = inputs[0];

  $: {
    for (let i of inputs) {
      if (i.id.toString() == view.input_id.toString()) {
        input = i;
      }
    }
  }

  let video: HTMLVideoElement;

  let width = 0;
  let height = 0;

  let canvas: HTMLCanvasElement;

  let handle = 0;

  // over element, clicking left, clicking right, x, y
  let mouse: [boolean, boolean, boolean, number, number] = [false, false, false, 0, 0];
  let dragStart: [number, number] = [0, 0];

  function submit() {
    sendMessage({
      type: 'update-view',
      data: view
    });
  }

  function resize() {
    let x = mouse[3];
    let y = mouse[4];
    let {top, right, bottom, left} = view.config;
    left = width - left;
    bottom = height - bottom;

    // right/left
    let middle = (right + left)/2;
    if (x < middle) {
      // right
      view.config.right = x;
    } else {
      // left
      view.config.left = width - x;
    }

    // top/bottom
    let center = (bottom + top)/2;
    if (y < center) {
      // top
      view.config.top = y;
    } else {
      // bottom
      view.config.bottom = height - y;
    }
  }

  function drag() {
    let x = mouse[3];
    let y = mouse[4];
    let dX = x - dragStart[0];
    let dY = y - dragStart[1];
    let {top, right, bottom, left} = view.config;

    top += dY;
    bottom -= dY;
    right += dX;
    left -= dX;

    if (top < 0) {
      bottom += top;
      top = 0;
    }
    if (right < 0) {
      left += right;
      right = 0;
    }
    if (bottom < 0) {
      top += bottom;
      bottom = 0;
    }
    if (left < 0) {
      right += left;
      left = 0;
    }
    view.config = {
      ...view.config,
      top, right, bottom, left
    };
    dragStart = [x, y];
  }

  function updateCurrent() {
    if (mouse[1]) resize();
    if (mouse[2]) drag();
  }

  function onMouseDown(e: MouseEvent) {
    if (e.button === 0) {
      mouse[1] = true;
    } else if (e.button === 2) {
      let rect = (e.target as HTMLCanvasElement).getBoundingClientRect();
      let x = e.clientX - rect.left;
      let y = e.clientY - rect.top;
      dragStart[0] = x;
      dragStart[1] = y;
      mouse[2] = true;
    }
    e.preventDefault();
    updateCurrent();
  }

  function onMouseUp(e: MouseEvent) {
    if (e.button === 0) {
      mouse[1] = false;
    } else if (e.button === 2) {
      mouse[2] = false;
    }
    submit();
    e.preventDefault();
  }

  function onMouseMove(e: MouseEvent) {
    let rect = (e.target as HTMLCanvasElement).getBoundingClientRect();
    let x = e.clientX - rect.left;
    let y = e.clientY - rect.top;

    mouse[3] = x;
    mouse[4] = y;

    updateCurrent();
  }

  function onMouseEnter() {
    mouse[0] = true;
  }

  function onMouseLeave() {
    if (mouse[1] || mouse[2]) {
      submit();
    }
    mouse[0] = false;
    mouse[1] = false;
    mouse[2] = false;
  }

  function onContextMenu(e: Event) {
    e.preventDefault();
  }

  onMount(() => {
    canvas.addEventListener('mousedown', onMouseDown, true);
    canvas.addEventListener('mouseup', onMouseUp, true);
    canvas.addEventListener('mousemove', onMouseMove, true);
    canvas.addEventListener('mouseenter', onMouseEnter, true);
    canvas.addEventListener('mouseleave', onMouseLeave, true);
    canvas.addEventListener('contextmenu', onContextMenu, true);

    width = video.width;
    height = video.height;
    canvas.width = width;
    canvas.height = height;

    let ctx = canvas.getContext("2d");

    function render_preview() {
      ctx.drawImage(video, 0, 0, width, height);

      let {top, right, bottom, left} = view.config;
      ctx.strokeStyle = "red";
      ctx.lineWidth = 5;
      ctx.strokeRect(right, top, width - left - right, height - bottom - top);
      handle = requestAnimationFrame(render_preview);
    }

    render_preview();
  });

  onDestroy(() => {
    cancelAnimationFrame(handle);
    canvas.removeEventListener('mousedown', onMouseDown);
    canvas.removeEventListener('mouseup', onMouseUp);
    canvas.removeEventListener('mousemove', onMouseMove);
    canvas.removeEventListener('mouseenter', onMouseEnter);
    canvas.removeEventListener('mouseleave', onMouseLeave);
    canvas.removeEventListener('contextmenu', onContextMenu);
  });

  function delete_view() {
    let yes = window.confirm("Are you sure you want to delete this view?");
    if (yes !== true) return;
    sendMessage({
      type: 'delete-view',
      data: {id: view.id},
    });
  }

  function change_input(e: Event) {
    let id = parseInt((e.target as HTMLSelectElement).value);
    view.input_id = id;
    submit();
  }

  function apply_preset(preset: Preset) {
    view.config = preset.config;
    submit();
    return false;
  }

  function save_preset() {
    let name = window.prompt("Enter a name for the preset.");
    if (name === undefined) return false;
    let presets = view.presets.filter(x => x.name !== name);
    presets.push({
      name,
      config: view.config,
    });
    view.presets = presets;
    submit();
  }

  let deletion_enabled = false;
  function delete_preset(preset: Preset) {
    deletion_enabled = false;
    let yes = window.confirm(`
    Are you sure you want to delete the preset '${preset.name}'?
    This cannot be undone!
    `);
    if (yes === true) {
      view.presets = view.presets.filter(x => x.name !== preset.name);
      submit();
    }
  }

</script>

<main>
  View Name: <input type="text" bind:value={view.name} on:change={submit}/>
  <button on:click={submit}>Sync</button>
  <button on:click={delete_view}>Delete</button>
  <br/>
  Input Stream:
  <!-- svelte-ignore a11y-no-onchange -->
  <select id="select_view" on:change={change_input}>
    {#each inputs as input}
      <option value={input.id.toString()}>{input.name}</option>
    {/each}
  </select>
  <br/>
  <video muted autoplay src={input.url} width={320} height={240} bind:this={video}/>
  <canvas
    bind:this={canvas}
    id="preview"/>
  <br/>
  <p>Left-click-and-drag to resize, right-click-and-drag to move.</p>
  <form action="">
    {#if video}
      <fieldset>
        <legend>Crop/Zoom</legend>
        <label for="top">Top</label>
        <input id="top" type="range" min={0} max={video.height}
                        bind:value={view.config.top} on:change={submit}/>
        <label for="right">Right</label>
        <input id="right" type="range" min={0} max={video.width}
                          bind:value={view.config.right} on:change={submit}/>
        <label for="bottom">Bottom</label>
        <input id="bottom" type="range" min={0} max={video.height}
                           bind:value={view.config.bottom} on:change={submit}/>
        <label for="left">Left</label>
        <input id="left" type="range" min={0} max={video.width}
                         bind:value={view.config.left} on:change={submit}/>
      </fieldset>
    {/if}
    <fieldset>
      <legend>Colors</legend>
      <label for="similarity">
        Compositing - Similarity: {view.config.compositing_color_similarity}/1000
      </label>
      <input id="similarity" type="range" min={0} max={1000}
                             bind:value={view.config.compositing_color_similarity} on:change={submit}/>
      <label for="temperature">
        Color Temperature: {view.config.color_temperature} K
      </label>
      <input id="temperature" type="range" min={1700} max={6500}
                              bind:value={view.config.color_temperature} on:change={submit}/>
    </fieldset>
    <fieldset>
      <legend>Presets</legend>
      <button type="button" on:click={save_preset}>Save</button>
      <br/>
      <fieldset>
        <legend>Restore</legend>
        {#each view.presets as preset}
          <button type="button" on:click={() => apply_preset(preset)}>{preset.name}</button>
        {/each}
      </fieldset>
      <br/>
      <label for="deletion_visible">Enable Delete Menu</label>
      <input id="deletion_visible" type="checkbox" bind:checked={deletion_enabled} title="Delete"/>
      <br/>
      <fieldset disabled={!deletion_enabled}>
        <legend>Delete</legend>
        {#each view.presets as preset}
          <button type="button" on:click={() => delete_preset(preset)}>{preset.name}</button>
        {/each}
      </fieldset>
    </fieldset>
  </form>
</main>

<style>
  button {
    margin: 2px;
  }

  main {
    display: inline-block;
    border: 1px solid black;
    padding: 25px;
  }

  canvas {
    border: 1px dashed black;
  }

  input {
    display: inline-block;
  }

  form > fieldset {
    /* width: 40%; */
    /* display: inline-block; */
    /* vertical-align: top; */
    float: left;
    margin: 10px;
    max-width: 25%;
  }
</style>
