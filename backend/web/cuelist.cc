#include "cuelist.hh"

CueList::CueList(int number, std::string name) : number_(number), name_(name), current_(8), baseline_(8) {}

int CueList::number() { return number_; }

std::string &CueList::name() { return name_; }

const int& CueList::cue() { return cue_; }

CueList::cue_t &CueList::levels() { return current_; }

CueList::fadelist_t &CueList::fades() { return fades_; }

const CueList::cue_t &CueList::baseline() { return baseline_; }

void CueList::save_as(int q, float time) {
  cues_[q] = current_;
  fades_[q] = time;
  baseline_ = cues_[q];
  cue_ = q;
}

void CueList::go_to(int q) {
  if (q == cue_ && is_fading_) {
    is_fading_ = false;
    current_ = baseline_ = cues_[cue_];
  }
  baseline_ = current_;
  cue_ = q;
  is_fading_ = true;
  fade_time_total_ = fades_[q];
  fade_time_left_ = fades_[q];
}

std::vector<int> CueList::cues() {
  std::vector<int> cues;
  for (auto const& x: cues_) {
    cues.push_back(x.first);
  }
  return cues;
}

void CueList::tick(int ms) {
  if (!is_fading_) return;
  fade_time_left_ -= ms/1000.f;
  CueList::cue_t next_q = cues_[cue_];

  if (fade_time_left_ < 0) {
    current_ = next_q;
    baseline_ = next_q;
    is_fading_ = false;
  }

  for (size_t i = 0; i < next_q.size(); i++) {
    int prev = baseline_[i];
    int next = next_q[i];
    float percent_left = fade_time_left_/fade_time_total_;
    current_[i] = prev*percent_left + next*(1-percent_left);
  }
}

bool CueList::is_fading() {
  return is_fading_;
}
