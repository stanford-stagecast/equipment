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

  shared_ptr<ssl::context> ctx = make_shared<ssl::context>(ssl::context::tlsv12);
  ctx->set_verify_mode(ssl::verify_none);
  ctx->use_certificate_chain_file("../../web/certificate/server.crt");
  ctx->use_private_key_file("../../web/certificate/server.key", ssl::context::pem);
  ctx->use_tmp_dh_file("../../web/certificate/dh1024.pem");

  net::io_context ioc;
  net::ip::address address = net::ip::make_address("0.0.0.0");
  tcp::endpoint endpoint(address, 8000);
  cout << "Listening at ws://localhost:8000..." << endl;

  shared_ptr<Dispatcher> dispatcher = make_shared<Dispatcher>();
  shared_ptr<Manager> manager = make_shared<Manager>(dispatcher, ioc, argv[1]);
  manager->begin();

  // we need a shared_ptr to exist for the listener to work
  make_shared<Listener>(ctx, ioc, endpoint, dispatcher)->run();

  ioc.run();
}
