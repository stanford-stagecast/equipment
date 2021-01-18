#ifndef CUELIST_HH
#define CUELIST_HH

#include "cue.hh"
#include <map>
#include <string>
#include <vector>

class CueList {
public:
  typedef struct {
    unsigned number;
    float fade_time;
  } cue_info_t;

  typedef enum {
    MANUAL,
    LOWERED,
    RAISED,
    TRACKED,
    BLOCKED,
  } cue_status_t;

  typedef struct {
    Cue::channel_t channel;
    Cue::level_t level;
    cue_status_t status;
  } level_info_t;

private:
  typedef enum {
    DEFAULT,
    FORCE_TRACK,
    FORCE_BLOCK,
  } tracking_state_t;

  std::vector<std::optional<Cue>> cues_{};
  unsigned current_cue_number_{0};

  std::map<Cue::channel_t, Cue::level_t> level_overrides_{};

  unsigned number_;
  std::string name_;

  unsigned max_known_channel_{7};

  bool is_fading_{false};
  float fade_time_left_{0};
  float fade_time_total_{1};
  Cue::levels_t fade_source_{};
  unsigned last_cue_number_{0};

  Cue::level_t get_tracked_level_at(int cue, Cue::channel_t channel);

public:
  CueList(unsigned number, std::string name);

  std::optional<float> fade_progress();
  void tick(int ms);

  std::vector<level_info_t> current_levels();
  void set_level(Cue::channel_t channel, std::optional<Cue::level_t> level);
  void track(Cue::channel_t channel);
  void block(Cue::channel_t channel);
  void back();
  void go();
  void go_to_cue(unsigned q);
  void record_cue(unsigned q, float time);
  void delete_cue(unsigned q);
  std::vector<cue_info_t> cue_info();

  float fade_time();

  unsigned cue() { return current_cue_number_; }
  unsigned next_cue();
  unsigned previous_cue();
  unsigned last_cue() { return last_cue_number_; }
  unsigned number() { return number_; }
  void set_name(std::string &name) { name_ = name; }
  std::string &name() { return name_; }
};

#endif
