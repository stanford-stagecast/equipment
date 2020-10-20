#ifndef CUELIST_HH
#define CUELIST_HH

#include <string>
#include <map>
#include <vector>

class CueList {
public:
  typedef std::vector<int> cue_t;
  typedef std::map<int, cue_t> cuelist_t;
  typedef std::map<int, float> fadelist_t;

  CueList(int number, std::string name);

  int number();
  std::string& name();

  const int& cue();
  cue_t& levels();
  fadelist_t& fades();
  const cue_t& baseline();
  void save_as(int q, float time);
  void go_to(int q);
  std::vector<int> cues();
  bool is_fading();

  void tick(int ms);

private:
  int number_;
  int cue_{0};
  std::string name_;

  cue_t current_;
  cue_t baseline_;

  cuelist_t cues_{};
  fadelist_t fades_{};

  bool is_fading_{false};
  float fade_time_left_{0};
  float fade_time_total_{0};
};

#endif
