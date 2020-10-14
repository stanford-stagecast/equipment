#ifndef MANAGER_HH
#define MANAGER_HH

#include <memory>
#include <map>
#include <string_view>
#include <boost/property_tree/ptree_fwd.hpp>
class Dispatcher;

class Manager : public std::enable_shared_from_this<Manager> {
private:
  std::map<int, int> levels_{};
  std::map<int, std::map<int, int>> cues_{};
  std::shared_ptr<Dispatcher> dispatcher_;
  int cue_ = 0;

  void get_levels();
  void set_levels(boost::property_tree::ptree values);
  void save_cue(int q);
  void restore_cue(int q);
  void list_cues();
public:
  Manager(std::shared_ptr<Dispatcher> dispatcher);

  void begin();
  void on_update(std::string_view update);
};

#endif
