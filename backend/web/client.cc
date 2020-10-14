#include "client.hh"
#include "listener.hh"
#include "dispatcher.hh"

#include <iostream>
#include <sstream>

Client::Client(tcp::socket &&socket,
               std::shared_ptr<Dispatcher> dispatcher)
    : ws_(std::move(socket)), buffer_(), queue_(), dispatcher_(dispatcher),
      write_in_progress_(false) {}

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
  dispatcher_->subscribe(weak_from_this());

  ws_.text(ws_.got_text());
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

  std::string s(net::buffer_cast<char *>(buffer_.data()), buffer_.size());
  dispatcher_->on_update(s);
  buffer_.consume(buffer_.size());
  do_read();
}

void Client::on_write(beast::error_code ec, std::size_t bytes) {
  boost::ignore_unused(bytes);

  if (ec) {
    return fail(ec, "write");
  }

  buffer_.consume(buffer_.size());

  write_in_progress_ = false;
  do_write();
}

void Client::do_update(std::string_view update) {
  queue_.push(std::string(update));
  do_write();
}

void Client::do_write() {
  if (write_in_progress_)
    return;
  if (queue_.empty())
    return;

  std::string value = queue_.front();
  write_in_progress_ = true;
  queue_.pop();
  ws_.async_write(
      net::buffer(value),
      beast::bind_front_handler(&Client::on_write, shared_from_this()));
}
