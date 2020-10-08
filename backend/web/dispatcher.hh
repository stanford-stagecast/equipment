#ifndef DISPATCHER_HH
#define DISPATCHER_HH

#include <mutex>
#include <vector>
#include <memory>
#include <string_view>

class Client;

class Dispatcher {
private:
  std::mutex mutex_;
  std::vector<std::weak_ptr<Client>> clients_;

public:
  Dispatcher();
  void subscribe(std::weak_ptr<Client> client);
  void on_update(std::string_view update);
};
#endif
