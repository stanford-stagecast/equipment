#include "cuelist.hh"

CueList::CueList(unsigned number, std::string name)
  : number_(number), name_(name) {
    max_known_channel_ = 8;
  }

std::vector<CueList::level_info_t> CueList::current_levels() {
  std::vector<level_info_t> levels;
  unsigned q = current_cue_number_;

  if (cues_.size() == 0) {
    // No cues saved; default to only manual overrides (if there are any).
    for (auto const &manual : level_overrides_) {
      level_info_t info;
      info.status = MANUAL;
      info.channel = manual.first;
      info.level = manual.second;
      levels.push_back(info);
    }
    return levels;
  }

  unsigned channels = max_known_channel_ + 1;

  for (unsigned i = 0; i < channels; i++) {
    Cue::level_t next = get_tracked_level_at(q, i);
    Cue::level_t previous = get_tracked_level_at(q - 1, i);
    level_info_t info;
    info.channel = i;

    Cue::level_t level = next;
    std::optional<float> fade = fade_progress();
    if (fade) {
      Cue::level_t old_level = 0;
      if (i < fade_source_.size()) {
        old_level = fade_source_[i].value_or(0);
      }
      level = (*fade) * next + (1 - (*fade)) * old_level;
    }

    info.level = level;

    if (!cues_[q]->get_level(i)) {
      info.status = TRACKED;
    } else if (level == previous) {
      info.status = BLOCKED;
    } else if (level > previous) {
      info.status = RAISED;
    } else if (level < previous) {
      info.status = LOWERED;
    }

    if (level_overrides_.count(i)) {
      Cue::level_t alt = level_overrides_[i];
      info.level = alt;
      info.status = MANUAL;
    }
    levels.push_back(info);
  }
  return levels;
}

void CueList::set_level(Cue::channel_t channel,
                        std::optional<Cue::level_t> level) {
  max_known_channel_ = std::max(max_known_channel_, channel);
  if (level.has_value()) {
    level_overrides_[channel] = *level;
  } else if (level_overrides_.count(channel)) {
    level_overrides_.erase(channel);
  }
}

void CueList::track(Cue::channel_t channel) {
  if (current_cue_number_ >= cues_.size())
    return;
  cues_[current_cue_number_]->set_level(channel, std::nullopt);
}

void CueList::block(Cue::channel_t channel) {
  if (current_cue_number_ >= cues_.size())
    return;
  cues_[current_cue_number_]->set_level(
      channel, get_tracked_level_at(current_cue_number_, channel));
}

void CueList::back() { go_to_cue(previous_cue()); }

void CueList::go() { go_to_cue(next_cue()); }

void CueList::go_to_cue(unsigned int q) {
  if (q >= cues_.size())
    return;

  if (q == current_cue_number_) {
    is_fading_ = false;
    return;
  }

  std::optional<Cue> cue = cues_[q];
  if (!cue)
    return;

  std::vector<level_info_t> current = current_levels();
  Cue::levels_t levels;
  for (auto const &info : current) {
    Cue::channel_t channel = info.channel;
    Cue::level_t level = info.level;
    if (channel >= levels.size()) {
      levels.resize(channel + 1);
    }
    levels[channel] = level;
  }
  fade_source_ = levels;

  level_overrides_.clear();
  last_cue_number_ = current_cue_number_;
  current_cue_number_ = q;
  fade_time_left_ = cue->fade_time();
  fade_time_total_ = cue->fade_time();
  is_fading_ = true;
  return;
}

void CueList::record_cue(unsigned q, float time) {
  std::vector<level_info_t> info = current_levels();
  std::optional<Cue> old_cue(std::nullopt);

  Cue::levels_t levels;
  Cue new_cue = Cue(time);
  for (auto const &x : info) {
    unsigned channel = x.channel;
    int level = x.level;
    if (level != get_tracked_level_at(q - 1, channel)) {
      new_cue.set_level(channel, level);
    }
  }

  for (auto const &x : level_overrides_) {
    unsigned channel = x.first;
    int level = x.second;
    new_cue.set_level(channel, level);
    if (level != get_tracked_level_at(q - 1, channel)) {
      new_cue.set_level(channel, level);
    }
  }

  if (q >= cues_.size()) {
    cues_.resize(q + 1);
  }
  std::optional<Cue> some_cue(new_cue);
  cues_[q].swap(some_cue);
  current_cue_number_ = q;
  level_overrides_.clear();
}

void CueList::delete_cue(unsigned q) {
  if (q > cues_.size())
    return;
  if (!cues_[q])
    return;
  if (q == current_cue_number_) {
    current_cue_number_ = previous_cue();
  }
  std::optional<Cue> none(std::nullopt);
  cues_[q].swap(none);
}

std::vector<CueList::cue_info_t> CueList::cue_info() {
  std::vector<CueList::cue_info_t> cues;
  for (size_t i = 0; i < cues_.size(); i++) {
    std::optional<Cue> cue = cues_[i];
    if (!cue)
      continue;
    cue_info_t info;
    info.number = i;
    info.fade_time = cue->fade_time();
    cues.push_back(info);
  }
  return cues;
}

void CueList::tick(int ms) {
  if (!is_fading_)
    return;
  fade_time_left_ -= ms / 1000.f;

  if (fade_time_left_ < 0) {
    is_fading_ = false;
  }
}

std::optional<float> CueList::fade_progress() {
  if (!is_fading_)
    return std::nullopt;
  return 1 - (fade_time_left_ / fade_time_total_);
}

Cue::level_t CueList::get_tracked_level_at(int cue,
                                           Cue::channel_t channel) {
  if (cue < 0) {
    return 0;
  }
  int current_cue = cue;
  if (static_cast<unsigned>(cue) >= cues_.size()) {
    current_cue = cues_.size() - 1;
  }
  while (current_cue >= 0) {
    std::optional<Cue> current = cues_[current_cue];
    if (current) {
      std::optional<Cue::level_t> level = current->get_level(channel);
      if (level) {
        return *level;
      }
    }
    current_cue--;
  }
  return 0;
}

unsigned CueList::next_cue() {
  unsigned prev = current_cue_number_;
  unsigned next = prev + 1;

  while (next < cues_.size()) {
    std::optional<Cue> cue = cues_[next];
    if (cue) {
      return next;
    }
    next++;
  }
  return prev;
}

unsigned CueList::previous_cue() {
  unsigned next = current_cue_number_;
  int prev = next - 1;

  while (prev >= 0) {
    std::optional<Cue> cue = cues_[prev];
    if (cue) {
      return prev;
    }
    prev--;
  }
  return next;
}
