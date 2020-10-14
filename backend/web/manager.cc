#include "dispatcher.hh"
#include "manager.hh"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace json = boost::property_tree;

Manager::Manager(std::shared_ptr<Dispatcher> dispatcher)
  : dispatcher_(dispatcher) {
  }

void Manager::begin() {
  dispatcher_->subscribe(weak_from_this());
}

void Manager::on_update(std::string_view update) {
  std::cout << "Got update: <" << update << ">" << std::endl;
  std::stringstream ss;
  ss << update;
  json::ptree pt;
  try {
    json::read_json(ss, pt);
  } catch (json::json_parser::json_parser_error& e) {
    std::cout << "Ignoring invalid JSON." << std::endl;
    return;
  }
}
