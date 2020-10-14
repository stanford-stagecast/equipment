#ifndef DISPATCHER_HH
#define DISPATCHER_HH

#include <mutex>
#include <vector>
#include <memory>
#include <string_view>

class Client;
class Manager;

class Dispatcher {
private:
  std::mutex client_mutex_{};
  std::vector<std::weak_ptr<Client>> clients_{};
  std::mutex manager_mutex_{};
  std::weak_ptr<Manager> manager_{};

public:
  Dispatcher();
  void subscribe(std::weak_ptr<Client> client);
  void subscribe(std::weak_ptr<Manager> manager);
  void on_update(std::string_view update);
  void do_update(std::string_view update);
};
#endif
