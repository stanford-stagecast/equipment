#include "dispatcher.hh"
#include "client.hh"
#include "manager.hh"

Dispatcher::Dispatcher() : mutex_(), clients_(), manager_() {}

void Dispatcher::subscribe(std::weak_ptr<Client> client) {
  std::lock_guard<std::mutex> lock(mutex_);
  clients_.push_back(client);
}

void Dispatcher::subscribe(std::weak_ptr<Manager> manager) {
  std::lock_guard<std::mutex> lock(mutex_);
  manager_ = manager;
}

void Dispatcher::on_update(std::string_view update) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (auto manager = manager_.lock()) {
    manager->on_update(update);
  }
}

void Dispatcher::do_update(std::string_view update) {
  std::vector<std::weak_ptr<Client>> v;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    v.reserve(clients_.size());
    for (auto it = clients_.begin(); it != clients_.end();) {
      if ((*it).expired()) {
        it = clients_.erase(it);
        continue;
      }
      v.emplace_back((*it));
      ++it;
    }
  }

  for (auto const &client_ptr : v) {
    if (auto client = client_ptr.lock()) {
      client->do_update(update);
    }
  }
}
