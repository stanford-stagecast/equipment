#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include "listener.hh"
#include "dispatcher.hh"
#include "manager.hh"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "Usage: web_backend <cue XML file>" << endl;
    exit(1);
  }

  net::io_context ioc;
  net::ip::address address = net::ip::make_address("0.0.0.0");
  tcp::endpoint endpoint(address, 8000);
  cout << "Listening at ws://localhost:8000..." << endl;

  shared_ptr<Dispatcher> dispatcher = make_shared<Dispatcher>();
  shared_ptr<Manager> manager = make_shared<Manager>(dispatcher, ioc, argv[1]);
  manager->begin();

  // we need a shared_ptr to exist for the listener to work
  make_shared<Listener>(ioc, endpoint, dispatcher)->run();

  ioc.run();
}
