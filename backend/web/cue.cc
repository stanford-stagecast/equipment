#include "cue.hh"

void Cue::set_level(channel_t channel, boost::optional<level_t> level) {
  if (levels_.size() <= channel) {
    levels_.resize(channel + 1);
  }
  levels_[channel] = level;
}

boost::optional<Cue::level_t> Cue::get_level(channel_t channel) {
  if (channel >= levels_.size()) {
    return boost::none;
  }
  return levels_[channel];
}

unsigned Cue::channels() {
  return levels_.size();
}

void Cue::extend(unsigned channels) {
  if (channels > levels_.size()) {
    levels_.resize(channels);
  }
}
