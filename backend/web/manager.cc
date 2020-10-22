#include "dispatcher.hh"
#include "manager.hh"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <queue>

static const unsigned CYCLE_TIME_MS = 25;

namespace json = boost::property_tree;

static const std::string GET_LEVELS = "get-levels";
static const std::string SET_LEVELS = "set-levels";
static const std::string RESET_CHANNEL = "reset-channel";
static const std::string TRACK_CHANNEL = "track-channel";
static const std::string BLOCK_CHANNEL = "block-channel";
static const std::string SAVE_CUE = "save-cue";
static const std::string RESTORE_CUE = "restore-cue";
static const std::string GO_CUE = "go-cue";
static const std::string BACK_CUE = "back-cue";
static const std::string DELETE_CUE = "delete-cue";
static const std::string LIST_CUES = "list-cues";

Manager::Manager(std::shared_ptr<Dispatcher> dispatcher, net::io_context& ioc)
  : dispatcher_(dispatcher), ioc_(ioc), timer_(ioc_, net::chrono::milliseconds(CYCLE_TIME_MS)) {
    tick_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    timer_.async_wait(boost::bind(&Manager::tick, this));
  }

void Manager::begin() {
  dispatcher_->subscribe(weak_from_this());
}

std::string status_message(CueList::cue_status_t status) {
  switch (status) {
    case(CueList::MANUAL):
      return "manual";
    case(CueList::LOWERED):
      return "lowered";
    case(CueList::RAISED):
      return "raised";
    case(CueList::TRACKED):
      return "tracked";
    case(CueList::BLOCKED):
      return "blocked";
  }
  std::cerr << "Unknown status: " << status << std::endl;
  return "unknown";
}

void Manager::get_levels() {
  json::ptree root;
  json::ptree values;
  json::ptree cue_info;

  for (auto info : list_.current_levels()) {
    json::ptree current;
    current.put("channel", info.channel);
    current.put("value", info.level);
    current.put("status", status_message(info.status));
    values.push_back(std::make_pair("", current));
  }

  {
    cue_info.put("current", list_.cue());
    cue_info.put("fade_time", list_.fade_time());
    cue_info.put("fade_progress", list_.fade_progress().value_or(1));
    cue_info.put("fading", list_.fade_progress().has_value());
    cue_info.put("last", list_.last_cue());
    cue_info.put("next", list_.next_cue());
    cue_info.put("previous", list_.previous_cue());
  }

  root.put("list", list_.number());
  root.put("type", GET_LEVELS);
  root.put_child("cue", cue_info);
  root.put_child("values", values);

  std::stringstream ss;
  json::write_json(ss, root);
  dispatcher_->do_update(std::string_view(ss.str()));
}

void Manager::set_levels(boost::property_tree::ptree values) {
  for (auto const& x : values) {
    json::ptree node = x.second;
    int channel = node.get<int>("channel");
    int value = node.get<int>("value");
    list_.set_level(channel, value);
  }
  get_levels();
}

void Manager::save_cue(unsigned q, float time) {
  list_.record_cue(q, time);
  list_cues();
  get_levels();
}

void Manager::tick() {
  std::chrono::milliseconds now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  if (list_.fade_progress()) {
    list_.tick((now - tick_time_).count());
    get_levels();
  }
  tick_time_ = now;
  timer_.expires_at(timer_.expires_at() + net::chrono::milliseconds(CYCLE_TIME_MS));
  timer_.async_wait(boost::bind(&Manager::tick, this));
}

void Manager::restore_cue(unsigned q) {
  list_.go_to_cue(q);
  get_levels();
}

void Manager::go_cue() {
  list_.go();
  get_levels();
}

void Manager::back_cue() {
  list_.back();
  get_levels();
}

void Manager::reset_channel(unsigned channel) {
  list_.set_level(channel, std::nullopt);
  get_levels();
}

void Manager::track_channel(unsigned channel) {
  list_.track(channel);
  get_levels();
}

void Manager::block_channel(unsigned channel) {
  list_.block(channel);
  get_levels();
}

void Manager::delete_cue(unsigned q) {
  list_.delete_cue(q);
  get_levels();
  list_cues();
}

void Manager::list_cues() {
  json::ptree root;
  json::ptree cues;
  for (auto const& x : list_.cue_info()) {
    json::ptree current;
    current.put("number", x.number);
    current.put("time", x.fade_time);
    cues.push_back(std::make_pair("", current));
  }
  root.put("type", LIST_CUES);
  root.put("cue", list_.cue());
  root.put_child("cues", cues);

  std::stringstream ss;
  json::write_json(ss, root);
  dispatcher_->do_update(std::string_view(ss.str()));
}

void Manager::on_update(std::string_view update) {
  std::stringstream ss;
  ss << update;
  json::ptree pt;
  try {
    json::read_json(ss, pt);
    std::string type = pt.get<std::string>("type");
    std::cout << "Got update of type: " << type << std::endl;
    if (type == GET_LEVELS) {
      get_levels();
    } else if (type == SET_LEVELS) {
      set_levels(pt.get_child("values"));
    } else if (type == RESET_CHANNEL) {
      reset_channel(pt.get<unsigned>("channel"));
    } else if (type == TRACK_CHANNEL) {
      track_channel(pt.get<unsigned>("channel"));
    } else if (type == BLOCK_CHANNEL) {
      block_channel(pt.get<unsigned>("channel"));
    } else if (type == SAVE_CUE) {
      save_cue(pt.get<int>("cue"), pt.get<float>("time"));
    } else if (type == RESTORE_CUE) {
      restore_cue(pt.get<int>("cue"));
    } else if (type == LIST_CUES) {
      list_cues();
    } else if (type == GO_CUE) {
      go_cue();
    } else if (type == BACK_CUE) {
      back_cue();
    } else if (type == DELETE_CUE) {
      delete_cue(pt.get<unsigned>("cue"));
    } else {
      std::cerr << "Invalid type: " << type << std::endl;
    }
  } catch (json::json_parser::json_parser_error& e) {
    std::cerr << "Ignoring invalid JSON. [" << e.what() << "]" << std::endl;
    return;
  } catch (json::ptree_bad_path& e) {
    std::cerr << "Ignoring incomplete JSON. [" << e.what() << "]" << std::endl;
    return;
  } catch (json::ptree_bad_data& e) {
    std::cerr << "Ignoring unparsable JSON. [" << e.what() << "]" << std::endl;
    return;
  }
}
