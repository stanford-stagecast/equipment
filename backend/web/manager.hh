#ifndef MANAGER_HH
#define MANAGER_HH

#include <memory>
#include <map>
#include <string_view>
#include <boost/property_tree/ptree_fwd.hpp>

#include "net.hh"

#include "cuelist.hh"
class Dispatcher;

class Manager : public std::enable_shared_from_this<Manager> {
private:
  std::shared_ptr<Dispatcher> dispatcher_;
  net::io_context& ioc_;
  CueList list_{0, ""};
  boost::asio::steady_timer timer_;
  std::chrono::milliseconds tick_time_{};

  void set_levels(boost::property_tree::ptree values);
  void save_cue(unsigned q, float time);
  void restore_cue(unsigned q);
  void delete_cue(unsigned q);
  void list_cues();
  void go_cue();
  void back_cue();

  void reset_channel(unsigned channel);
  void track_channel(unsigned channel);
  void block_channel(unsigned channel);

public:
  Manager(std::shared_ptr<Dispatcher> dispatcher, net::io_context& ioc);

  void begin();
  void on_update(std::string_view update);
  void get_levels();
  void tick();
};

#endif
