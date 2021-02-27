#include "manager.hh"
#include "dispatcher.hh"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <optional>
#include <queue>

using namespace std;

static const unsigned CYCLE_TIME_MS = 25;

namespace json = boost::property_tree;

// List operations
static const string GET_LISTS = "get-lists";
static const string RENAME_LIST = "rename-list";
static const string DELETE_LIST = "delete-list";

// Cue operations
static const string LIST_CUES = "list-cues";
static const string GET_LEVELS = "get-levels";
static const string SET_LEVELS = "set-levels";
static const string SET_MUTE = "set-mute";
static const string SAVE_CUE = "save-cue";
static const string RESTORE_CUE = "restore-cue";

// Channel operations
static const string RESET_CHANNEL = "reset-channel";
static const string TRACK_CHANNEL = "track-channel";
static const string BLOCK_CHANNEL = "block-channel";
static const string SHOW_CHANNEL = "show-channel";
static const string HIDE_CHANNEL = "hide-channel";

// Show Control
static const string GO_CUE = "go-cue";
static const string BACK_CUE = "back-cue";
static const string DELETE_CUE = "delete-cue";
static const string SAVE_TO_DISK = "save-to-disk";

Manager::Manager(shared_ptr<Dispatcher> dispatcher, net::io_context &ioc, std::string filename, int dmx_port)
    : dispatcher_(dispatcher), ioc_(ioc),
      timer_(ioc_, net::chrono::milliseconds(CYCLE_TIME_MS)),
      transmitter_(ioc, net::ip::make_address("127.0.0.1"), dmx_port),
      filename_(filename) {
  tick_time_ = chrono::duration_cast<chrono::milliseconds>(
      chrono::system_clock::now().time_since_epoch());
  timer_.async_wait(boost::bind(&Manager::tick, this));

  std::ifstream ifs(filename_);
  if (!ifs.is_open()) {
    cout << "Unable to find input file.  Initializing with empty state." << endl;
    return;
  }
  try {

  } catch (boost::archive::archive_exception& e) {
    cerr << e.what() << endl;
    cerr << "Could not process saved cues." << endl;
    exit(1);
  }
}

void Manager::begin() { dispatcher_->subscribe(weak_from_this()); }

void Manager::save_to_disk() {
  std::ofstream ofs(filename_);
  for (auto list : lists_) {
      stringstream ss;
      json::write_json(ss, make_cuelist_json(list.second));
      ofs << ss.str();
  }
  ofs.close();
}

string status_message(CueList::cue_status_t status) {
  switch (status) {
  case (CueList::MANUAL):
    return "manual";
  case (CueList::LOWERED):
    return "lowered";
  case (CueList::RAISED):
    return "raised";
  case (CueList::TRACKED):
    return "tracked";
  case (CueList::BLOCKED):
    return "blocked";
  }
  cerr << "Unknown status: " << status << endl;
  return "unknown";
}

json::ptree Manager::make_cuelist_json(CueList &list_) {
    json::ptree root;
    json::ptree cues;
    root.put("list", list_.number());
    list_.go_to_cue(0); // Scroll the list back

    while (list_.cue() != list_.next_cue()) {
        json::ptree cue = make_cue_json(list_);
        cues.put_child(to_string(list_.cue()), cue);
        list_.go();
    }
    if (list_.cue() != 0) {
        json::ptree cue = make_cue_json(list_);
        cues.put_child(to_string(list_.cue()), cue);
    }
    root.put_child("cues", cues);
    return root;
}

json::ptree Manager::make_cue_json(CueList &list_) {
    json::ptree root;
    json::ptree values;
    json::ptree cue_info;

    for (auto info : list_.current_levels()) {
      json::ptree current;
      current.put("channel", info.channel);
      current.put("value", clamp(info.level.pan / 255.0, 0.0, 1.0));
      current.put("mute", info.level.mute);
      current.put("status", status_message(info.status));
      current.put("visible", info.visible);
      // if (info.channel < 512) {
      //   universe[info.channel] = clamp(info.level, 0, 1);
      // }
      values.push_back(make_pair("", current));
    }

    {
      cue_info.put("current", list_.cue());
      cue_info.put("fade_time", list_.fade_time());
      cue_info.put("fade_progress", list_.fade_progress().value_or(1));
      cue_info.put("fading", list_.fade_progress().has_value());
      cue_info.put("last", list_.last_cue());
      cue_info.put("next", list_.next_cue());
      cue_info.put("previous", list_.previous_cue());
      cue_info.put_child("values", values);
    }

    root.put("list", list_.number());
    root.put("type", GET_LEVELS);
    root.put_child("cue", cue_info);

    return root;
}

void Manager::get_levels(CueList &list_) {
  Transmitter::universe_t universe = {0};

  stringstream ss;
  json::write_json(ss, make_cue_json(list_));
  transmitter_.update(list_.number(), universe);
  dispatcher_->do_update(string_view(ss.str()));
}

void Manager::set_levels(CueList &list_, boost::property_tree::ptree values) {
  for (auto &x : values) {
    json::ptree node = x.second;
    int channel = node.get<int>("channel");
    Cue::level_t level = Cue::DEFAULT_LEVEL;
    level.pan = node.get<int>("value");
    level.mute = node.get<bool>("mute");
    list_.set_level(channel, level);
  }
  get_levels(list_);
}

// void Manager::set_mutes(CueList &list_, boost::property_tree::ptree values) {
// 	for (auto &x : values) {
// 	  json::ptree node = x.second;
// 	  int channel = node.get<int>("channel");
// 	  bool mute = node.get<bool>("mute");
// 	  list_.set_mute(channel, mute);
// 	}
// 	get_levels(list_);
// }

void Manager::save_cue(CueList &list_, unsigned q, float time) {
  list_.record_cue(q, time);
  list_cues(list_);
  get_levels(list_);
}

void Manager::tick() {
  chrono::milliseconds now = chrono::duration_cast<chrono::milliseconds>(
      chrono::system_clock::now().time_since_epoch());
  for (auto &x : lists_) {
    CueList &list_ = x.second;
    if (list_.fade_progress().has_value()) {
      list_.tick((now - tick_time_).count());
      get_levels(list_);
    }
  }
  transmitter_.tick((now - tick_time_).count());
  tick_time_ = now;
  timer_.expires_at(timer_.expires_at() +
                    net::chrono::milliseconds(CYCLE_TIME_MS));
  timer_.async_wait(boost::bind(&Manager::tick, this));
}

void Manager::restore_cue(CueList &list_, unsigned q) {
  list_.go_to_cue(q);
  get_levels(list_);
}

void Manager::go_cue(CueList &list_) {
  list_.go();
  get_levels(list_);
}

void Manager::back_cue(CueList &list_) {
  list_.back();
  get_levels(list_);
}

void Manager::reset_channel(CueList &list_, unsigned channel) {
  list_.set_level(channel, boost::none);
  get_levels(list_);
}

void Manager::track_channel(CueList &list_, unsigned channel) {
  list_.track(channel);
  get_levels(list_);
}

void Manager::block_channel(CueList &list_, unsigned channel) {
  list_.block(channel);
  get_levels(list_);
}

void Manager::show_channel(CueList &list_, unsigned channel) {
  list_.show(channel);
  get_levels(list_);
}

void Manager::hide_channel(CueList &list_, unsigned channel) {
  list_.hide(channel);
  get_levels(list_);
}

void Manager::delete_cue(CueList &list_, unsigned q) {
  list_.delete_cue(q);
  get_levels(list_);
  list_cues(list_);
}

void Manager::list_cues(CueList &list_) {
  json::ptree root;
  json::ptree cues;
  for (auto &x : list_.cue_info()) {
    json::ptree current;
    current.put("number", x.number);
    current.put("time", x.fade_time);
    cues.push_back(make_pair("", current));
  }
  root.put("type", LIST_CUES);
  root.put("cue", list_.cue());
  root.put("list", list_.number());
  root.put_child("cues", cues);

  stringstream ss;
  json::write_json(ss, root);
  dispatcher_->do_update(string_view(ss.str()));
}

void Manager::get_lists() {
  json::ptree root;
  json::ptree lists;
  for (auto &x : lists_) {
    CueList &l = x.second;
    json::ptree current;
    current.put("number", l.number());
    current.put("name", l.name());
    lists.push_back(make_pair("", current));
  }
  root.put("type", GET_LISTS);
  root.put_child("lists", lists);

  stringstream ss;
  json::write_json(ss, root);
  dispatcher_->do_update(string_view(ss.str()));
}

void Manager::rename_list(CueList &list, string &name) {
  list.set_name(name);
  get_lists();
}

void Manager::delete_list(CueList &list) {
  lists_.erase(list.number());
  transmitter_.remove_universe(list.number());
  get_lists();
}

void Manager::on_update(string_view update) {
  stringstream ss;
  ss << update;
  json::ptree pt;
  try {
    json::read_json(ss, pt);
    string type = pt.get<string>("type");
    cout << "Got update of type: " << type;

    if (type == GET_LISTS) {
      cout << endl;
      get_lists();
      return;
    } else if (type == SAVE_TO_DISK) {
      cout << endl;
      save_to_disk();
      return;
    }

    unsigned list_id = pt.get<unsigned>("list_id");
    cout << " " << list_id << endl;

    if (lists_.count(list_id) != 1) {
      CueList l(list_id, "untitled");
      lists_.insert({list_id, l});
    }
    auto found = lists_.find(list_id);
    CueList &list = (*found).second;

    if (type == RENAME_LIST) {
      string name = pt.get<string>("name");
      rename_list(list, name);
    } else if (type == DELETE_LIST) {
      delete_list(list);
    } else if (type == GET_LEVELS) {
      get_levels(list);
    } else if (type == SET_LEVELS) {
      set_levels(list, pt.get_child("values"));
  	} else if (type == SET_MUTE) {
	  // set_mutes(list, pt.get_child("values"));
    } else if (type == RESET_CHANNEL) {
      reset_channel(list, pt.get<unsigned>("channel"));
    } else if (type == TRACK_CHANNEL) {
      track_channel(list, pt.get<unsigned>("channel"));
    } else if (type == BLOCK_CHANNEL) {
      block_channel(list, pt.get<unsigned>("channel"));
    } else if (type == SHOW_CHANNEL) {
      show_channel(list, pt.get<unsigned>("channel"));
    } else if (type == HIDE_CHANNEL) {
      hide_channel(list, pt.get<unsigned>("channel"));
    } else if (type == SAVE_CUE) {
      save_cue(list, pt.get<int>("cue"), pt.get<float>("time"));
    } else if (type == RESTORE_CUE) {
      restore_cue(list, pt.get<int>("cue"));
    } else if (type == LIST_CUES) {
      list_cues(list);
    } else if (type == GO_CUE) {
      go_cue(list);
    } else if (type == BACK_CUE) {
      back_cue(list);
    } else if (type == DELETE_CUE) {
      delete_cue(list, pt.get<unsigned>("cue"));
    } else {
      cerr << "Invalid type: " << type << endl;
    }
  } catch (json::json_parser::json_parser_error &e) {
    cerr << "Ignoring invalid JSON. [" << e.what() << "]" << endl;
    return;
  } catch (json::ptree_bad_path &e) {
    cerr << "Ignoring incomplete JSON. [" << e.what() << "]" << endl;
    return;
  } catch (json::ptree_bad_data &e) {
    cerr << "Ignoring unparsable JSON. [" << e.what() << "]" << endl;
    return;
  }
}
