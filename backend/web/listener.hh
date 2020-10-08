#ifndef LISTENER_HH
#define LISTENER_HH

#include <string_view>

#include "beast.hh"
#include "net.hh"

class Listener : public std::enable_shared_from_this<Listener> {
private:
  net::io_context& ioc_;
  tcp::acceptor acceptor_;

  void fail(beast::error_code ec, std::string_view what);

  void do_accept();
  void on_accept(beast::error_code ec, tcp::socket socket);

public:
  Listener(net::io_context& ioc, tcp::endpoint endpoint);

  void run();
};

#endif
