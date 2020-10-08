#include "listener.hh"
#include "client.hh"
#include "dispatcher.hh"

#include <iostream>

Listener::Listener(net::io_context &ioc, tcp::endpoint endpoint, std::shared_ptr<Dispatcher> dispatcher)
    : ioc_(ioc), acceptor_(ioc), dispatcher_(dispatcher) {
  beast::error_code ec;
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
    return;
  }

  acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
    return;
  }

  acceptor_.bind(endpoint, ec);
  if (ec) {
    fail(ec, "bind");
    return;
  }

  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
    return;
  }
}

void Listener::run() { do_accept(); }

void Listener::do_accept() {
  acceptor_.async_accept(
      net::make_strand(ioc_),
      beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void Listener::fail(beast::error_code ec, std::string_view what) {
  std::cerr << "Listener -- Error in `" << what << "`: " << ec.message()
            << std::endl;
}

void Listener::on_accept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    return fail(ec, "accept");
  }

  std::make_shared<Client>(std::move(socket), dispatcher_)->run();

  do_accept();
}
