#include "client.hh"
#include "listener.hh"

#include <iostream>

Client::Client(tcp::socket &&socket) : ws_(std::move(socket)), buffer_() {}

Client::~Client() {}

void Client::run() {
  ws_.async_accept(
      beast::bind_front_handler(&Client::on_accept, shared_from_this()));
}

void Client::do_read() {
  ws_.async_read(
      buffer_, beast::bind_front_handler(&Client::on_read, shared_from_this()));
}

void Client::fail(beast::error_code ec, std::string_view what) {
  std::cerr << "Client -- Error in `" << what << "`: " << ec.message()
            << std::endl;
}

void Client::on_accept(beast::error_code ec) {
  if (ec) {
    return fail(ec, "accept");
  }

  do_read();
}

void Client::on_read(beast::error_code ec, std::size_t bytes) {
  boost::ignore_unused(bytes);

  if (ec == websocket::error::closed) {
    return;
  }

  if (ec) {
    return fail(ec, "read");
  }

  ws_.text(ws_.got_text());
  ws_.async_write(buffer_.data(), beast::bind_front_handler(
                                      &Client::on_write, shared_from_this()));
}

void Client::on_write(beast::error_code ec, std::size_t bytes) {
  boost::ignore_unused(bytes);

  if (ec) {
    return fail(ec, "write");
  }

  buffer_.consume(buffer_.size());

  do_read();
}
