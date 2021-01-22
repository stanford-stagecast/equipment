#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include "listener.hh"
#include "dispatcher.hh"
#include "manager.hh"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 4) {
    cout << "Usage: web_backend <cue XML file> <websocket port> <DMX UDP port>" << endl;
    exit(1);
  }
  string cues = argv[1];
  int ws = stoi(argv[2]);
  int dmx = stoi(argv[3]);

  net::io_context ioc;
  net::ip::address address = net::ip::make_address("0.0.0.0");
  tcp::endpoint endpoint(address, ws);
  cout << "Listening at ws://0.0.0.0:" << endpoint.port() << endl;

  shared_ptr<Dispatcher> dispatcher = make_shared<Dispatcher>();
  shared_ptr<Manager> manager = make_shared<Manager>(dispatcher, ioc, cues, dmx);
  manager->begin();

  // we need a shared_ptr to exist for the listener to work
  make_shared<Listener>(ioc, endpoint, dispatcher)->run();

  ioc.run();
}
