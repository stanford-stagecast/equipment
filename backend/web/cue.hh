#ifndef CUE_HH
#define CUE_HH

#include <vector>
#include "serialization.hh"
#include <boost/optional.hpp>

class Cue {
public:
    typedef struct{
        int pan;
        bool mute;
        // level_t(int pan_init=128, bool mute_init=true) : pan{pan_init}, mute{mute_init} {};
        // bool operator==(const level_t rhs) {
        //   return (pan == rhs.pan) && (mute == rhs.mute);
        // }
    } level_t;

  static constexpr level_t DEFAULT_LEVEL = {128, true};

  typedef unsigned channel_t;
  typedef std::vector<boost::optional<level_t>> levels_t;

private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    (void) version;
    ar & BOOST_SERIALIZATION_NVP(levels_);
    ar & BOOST_SERIALIZATION_NVP(fade_time_);
  }

  levels_t levels_{};
  float fade_time_{0};

public:
  Cue(float fade_time): fade_time_(fade_time) {}
  Cue(): fade_time_(3) {}

  void set_level(channel_t channel, boost::optional<level_t> level);
  boost::optional<level_t> get_level(channel_t channel);

  const levels_t& levels() { return levels_; };
  float fade_time() { return fade_time_; };

  unsigned channels();
  void extend(unsigned channels);
};

#endif
