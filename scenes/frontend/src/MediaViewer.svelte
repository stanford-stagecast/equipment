<script lang="ts">
  import type { Media } from './types';

  export let media: Media;

</script>

<main>
  <li>{media.name} ({media.type})</li>
</main>

<style>
  main {
    margin: 10px;
  }
</style>
