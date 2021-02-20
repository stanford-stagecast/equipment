<script lang="ts">
  import type {State} from './types';
  import {WIDTH, HEIGHT} from './types';
  import { onMount, onDestroy } from 'svelte';

  export let state: State;
  export let cue: Cue;
  export let current: number;

  let canvas: HTMLCanvasElement;
  let handle: number;

  let elements = [];

  onMount(() => {
    handle = requestAnimationFrame(paint);
  });

  onDestroy(() => {
    cancelAnimationFrame(handle);
  });

  function paint() {
    let [w, h] = [canvas.width, canvas.height];
    let [w_scale, h_scale] = [w / WIDTH, h / HEIGHT];
    let ctx = canvas.getContext("2d");
    handle = requestAnimationFrame(paint);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, w, h);
    if (!elements) return;

    current = current || 0;
    let current_change = cue.changes[current];
    let current_type = current_change.type;
    let current_id;
    if (current_type === "media") {
      current_id = current_change.media_id;
    } else if (current_type === "camera") {
      current_id = current_change.camera_id;
    }
    current_id = current_id.toString();

    for (let x of elements) {
      if (!x.config) continue;
      let view = x.config.view;
      if (x.type === current_type && x.id === current_id) {
        ctx.fillStyle = "blue";
        ctx.fillRect(view.left, view.top, view.right - view.left, view.bottom - view.top);
      } else {
        ctx.strokeStyle = "white";
        ctx.lineWidth = 5;
        ctx.strokeRect(view.left, view.top, view.right - view.left, view.bottom - view.top);
      }
    }
  }

  $: {
    let visible = {};
    let playing = {};

    let current = 0;
    while (current < state.cues.length && state.cues[current].number <= cue.number) {
      for (let change of state.cues[current].changes) {
        if (change.type === "media") {
          if (change.action.type === "add") {
            playing[change.media_id] = change.action.config;
          } else if (change.action.type === "remove") {
            playing[change.media_id] = undefined;
          } else if (change.action.type === "update") {
            playing[change.media_id] = change.action.config;
          }
        } else if (change.type === "linked") {
        } else if (change.type === "camera") {
          if (change.action.type === "add") {
            visible[change.camera_id] = change.action.config;
          } else if (change.action.type === "remove") {
            visible[change.camera_id] = undefined;
          } else if (change.action.type === "update") {
            visible[change.camera_id] = change.action.config;
          }
        }
      }
      current += 1;
    }

    elements = [];
    for (let key of Object.keys(playing)) {
      if (!playing[key]) continue;
      elements.push({type: "media", id: key, config: playing[key]});
    }
    for (let key of Object.keys(visible)) {
      if (!visible[key]) continue;
      elements.push({type: "camera", id: key, config: visible[key]});
    }
    elements = elements.sort((a, b) => b.config.view.z - a.config.view.z);
  }
</script>

<main>
  <canvas bind:this={canvas} width={WIDTH} height={HEIGHT}>
  </canvas>
</main>

<style>
  canvas {
    border: 1px solid black;
  }

  canvas {
    width: 640px;
    height: 360px;
  }
</style>
