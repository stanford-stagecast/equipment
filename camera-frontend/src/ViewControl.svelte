<script lang="ts">
  import type {Output} from './types';
  import { onMount, onDestroy } from 'svelte';

  export let output: Output;
  let video: HTMLVideoElement;

  let width = 0;
  let height = 0;

  let canvas: HTMLCanvasElement;

  let handle = 0;

  // over element, clicking left, clicking right, x, y
  let mouse: [boolean, boolean, boolean, number, number] = [false, false, false, 0, 0];
  let dragStart: [number, number] = [0, 0];

  function resize() {
    let x = mouse[3];
    let y = mouse[4];
    let {top, right, bottom, left} = output.current;
    left = width - left;
    bottom = height - bottom;

    // right/left
    let middle = (right + left)/2;
    if (x < middle) {
      // right
      output.current.right = x;
    } else {
      // left
      output.current.left = width - x;
    }

    // top/bottom
    let center = (bottom + top)/2;
    if (y < center) {
      // top
      output.current.top = y;
    } else {
      // bottom
      output.current.bottom = height - y;
    }
  }

  function drag() {
    let x = mouse[3];
    let y = mouse[4];
    let dX = x - dragStart[0];
    let dY = y - dragStart[1];
    let {top, right, bottom, left} = output.current;

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
    output.current = {
      top, right, bottom, left
    };
    dragStart = [x, y];
  }

  function updateCurrent() {
    if (mouse[1]) resize();
    if (mouse[2]) drag();
  }

  function onMouseDown(e) {
    if (e.button === 0) {
      mouse[1] = true;
    } else if (e.button === 2) {
      let rect = e.target.getBoundingClientRect();
      let x = e.clientX - rect.left;
      let y = e.clientY - rect.top;
      dragStart[0] = x;
      dragStart[1] = y;
      mouse[2] = true;
    }
    e.preventDefault();
    updateCurrent();
  }

  function onMouseUp(e) {
    if (e.button === 0) {
      mouse[1] = false;
    } else if (e.button === 2) {
      mouse[2] = false;
    }
    e.preventDefault();
  }

  function onMouseMove(e) {
    let rect = e.target.getBoundingClientRect();
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
    mouse[0] = false;
    mouse[1] = false;
    mouse[2] = false;
  }

  function onContextMenu(e) {
    e.preventDefault();
  }

  export const start = ((video_elem) => {
    video = video_elem;
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

      let {top, right, bottom, left} = output.current;
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

  function applyPreset(preset) {
    output.current = preset.config;
  }

  function savePreset(name) {
    let done = false;
    for (let preset of output.presets) {
      if (preset.name === name) {
        preset.config = output.current;
      }
    }
  }
</script>

<main>
  <canvas
    bind:this={canvas}
    id="preview"/>
  <br/>
  {#each output.presets as preset}
    <button on:click={() => applyPreset(preset)}>Switch to "{preset.name}"</button>
  {/each}
  <p>Left-click-and-drag to resize, right-click-and-drag to move.</p>
  {#each output.presets as preset}
    <button on:click={() => savePreset(preset.name)}>Record as "{preset.name}"</button>
  {/each}
</main>

<style>
  button {
    margin: 2px;
  }

  main {
    display: inline-block;
  }

  canvas {
    border: 1px dashed black;
  }
</style>
