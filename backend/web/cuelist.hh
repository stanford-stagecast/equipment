#ifndef CUELIST_HH
#define CUELIST_HH

#include <string>
#include <map>
#include <vector>

class CueList {
public:
  typedef std::map<int, int> cue_t;
  typedef std::map<int, cue_t> cuelist_t;

  CueList(int number, std::string name);

  int number();
  std::string& name();

  const int& cue();
  cue_t& levels();
  const cue_t& baseline();
  void save_as(int q);
  void go_to(int q);
  std::vector<int> cues();

private:
  int number_;
  int cue_{0};
  std::string name_;
  cue_t current_{};
  cue_t baseline_{};
  cuelist_t cues_{};

};

#endif
