#ifndef CUE_HH
#define CUE_HH

#include <vector>
#include <optional>

class Cue {
public:
  typedef unsigned channel_t;
  typedef int level_t;
  typedef std::vector<std::optional<level_t>> levels_t;

private:
  levels_t levels_{};
  float fade_time_{0};

public:
  Cue(float fade_time): fade_time_(fade_time) {}

  void set_level(channel_t channel, std::optional<level_t> level);
  std::optional<level_t> get_level(channel_t channel);

  const levels_t& levels() { return levels_; };
  float fade_time() { return fade_time_; };

  unsigned channels();
  void extend(unsigned channels);
};

#endif
