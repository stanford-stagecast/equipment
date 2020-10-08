#include "listener.hh"
#include "dispatcher.hh"
#include <iostream>

int main() {
  net::io_context ioc;
  net::ip::address address = net::ip::make_address("0.0.0.0");
  tcp::endpoint endpoint(address, 8000);

  std::shared_ptr<Dispatcher> dispatcher = std::make_shared<Dispatcher>();

  // we need a shared_ptr to exist for the listener to work
  std::make_shared<Listener>(ioc, endpoint, dispatcher)->run();

  ioc.run();
}
