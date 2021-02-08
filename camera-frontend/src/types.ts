export type State = {
  inputs?: Input[],
  views?: View[],
};

export type Input = {
  name: string, // the user-facing name of this input stream
  id: number, // a unique number associated with this input
  url: string, // URL to a playable video stream containing a preview of this input
};

export type View = {
  name: string, // the user-facing name of this view
  id: number, // a unique number associated with this view
  input_id: number, // the id of the input this view is based on
  config: Config, // the current configuration
  presets: Preset[], // saved "preset" configurations
}

export type Config = {
  top: number, // Offset in pixels from top
  right: number, // Offset in pixels from right
  bottom: number, // Offset in pixels from bottom
  left: number, // Offset in pixels from left
  color_temperature: number, // Color Temperature in Kelvin; ~1700-6500
  compositing_color_similarity: number, // Green screening color similarity (0-1000)
};

export type Preset = {
  name: string, // the user-facing name of this preset (used as a unique key)
  config: Config, // the saved configuration
};
