#include "dispatcher.hh"
#include "manager.hh"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <queue>

namespace json = boost::property_tree;

static const std::string GET_LEVELS = "get-levels";
static const std::string SET_LEVELS = "set-levels";
static const std::string SAVE_CUE = "save-cue";
static const std::string RESTORE_CUE = "restore-cue";
static const std::string LIST_CUES = "list-cues";

Manager::Manager(std::shared_ptr<Dispatcher> dispatcher, net::io_context& ioc)
  : dispatcher_(dispatcher), ioc_(ioc), timer_(ioc_, net::chrono::milliseconds(25)) {
    timer_.async_wait(boost::bind(&Manager::tick, this));
  }

void Manager::begin() {
  dispatcher_->subscribe(weak_from_this());
}

void Manager::get_levels() {
  json::ptree root;
  json::ptree values;
  CueList::cue_t levels = list_.levels();
  CueList::cue_t baseline = list_.baseline();

  for (size_t i = 0; i < levels.size(); i++) {
    json::ptree current;
    int channel = i;
    int value = levels[i];
    current.put("channel", channel);
    current.put("value", value);
    std::string state = "changed";
    if (baseline[channel] == value) {
      state = "saved";
    }
    if (list_.is_fading()) {
      if (baseline[channel] < value) {
        state = "rising";
      } else if (baseline[channel] > value) {
        state = "falling";
      }
    }
    current.put("status", state);
    values.push_back(std::make_pair("", current));
  }
  root.put("list", list_.number());
  root.put("type", GET_LEVELS);
  root.put("cue", list_.cue());
  root.put_child("values", values);

  std::stringstream ss;
  json::write_json(ss, root);
  dispatcher_->do_update(std::string_view(ss.str()));
}

void Manager::set_levels(boost::property_tree::ptree values) {
  CueList::cue_t data(list_.levels());
  for (auto const& x : values) {
    json::ptree node = x.second;
    int channel = node.get<int>("channel");
    int value = node.get<int>("value");
    data[channel] = value;
  }
  list_.levels().swap(data);
  get_levels();
}

void Manager::save_cue(int q) {
  list_.save_as(q, 3);
  list_cues();
  get_levels();
}

void Manager::tick() {
  if (list_.is_fading()) {
    list_.tick(25);
    get_levels();
  }
  timer_.expires_at(timer_.expires_at() + net::chrono::milliseconds(25));
  timer_.async_wait(boost::bind(&Manager::tick, this));
}

void Manager::restore_cue(int q) {
  list_.go_to(q);
}

void Manager::list_cues() {
  json::ptree root;
  json::ptree cues;
  for (int const& x: list_.cues()) {
    json::ptree current;
    current.put("number", x);
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
    } else if (type == SAVE_CUE) {
      save_cue(pt.get<int>("cue"));
    } else if (type == RESTORE_CUE) {
      restore_cue(pt.get<int>("cue"));
    } else if (type == LIST_CUES) {
      list_cues();
    } else {
      std::cout << "Invalid type: " << type << std::endl;
    }
  } catch (json::json_parser::json_parser_error& e) {
    std::cout << "Ignoring invalid JSON. [" << e.what() << "]" << std::endl;
    return;
  } catch (json::ptree_bad_path& e) {
    std::cout << "Ignoring incomplete JSON. [" << e.what() << "]" << std::endl;
    return;
  }
}
