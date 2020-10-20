#include "cuelist.hh"

CueList::CueList(int number, std::string name) : number_(number), name_(name) {}

int CueList::number() { return number_; }

std::string &CueList::name() { return name_; }

const int& CueList::cue() { return cue_; }

CueList::cue_t &CueList::levels() { return current_; }

const CueList::cue_t &CueList::baseline() { return baseline_; }

void CueList::save_as(int q) {
  cues_[q] = current_;
  baseline_ = cues_[q];
  cue_ = q;
}

void CueList::go_to(int q) {
  current_ = baseline_ = cues_[q];
  cue_ = q;
}

std::vector<int> CueList::cues() {
  std::vector<int> cues;
  for (auto const& x: cues_) {
    cues.push_back(x.first);
  }
  return cues;
}
