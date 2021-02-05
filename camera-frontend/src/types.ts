export type ViewConfig = {
  top: number,
  right: number,
  bottom: number,
  left: number,
};

export type Output = {
  name: string,
  current: ViewConfig,
  presets: [{
    name: string,
    config: ViewConfig,
  }]
};

export type Actor = {
  name: string,
  url: string,
  outputs: Output[],
};
