#ifndef MANAGER_HH
#define MANAGER_HH

#include <memory>
#include <string_view>
class Dispatcher;

class Manager : public std::enable_shared_from_this<Manager> {
private:
  std::shared_ptr<Dispatcher> dispatcher_;
public:
  Manager(std::shared_ptr<Dispatcher> dispatcher);

  void begin();
  void on_update(std::string_view update);
};

#endif
