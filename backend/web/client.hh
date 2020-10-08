#ifndef CLIENT_HH
#define CLIENT_HH

#include <string_view>

#include "net.hh"
#include "beast.hh"

class Client : public std::enable_shared_from_this<Client> {
private:
  websocket::stream<beast::tcp_stream> ws_;
  beast::flat_buffer buffer_;

  void fail(beast::error_code ec, std::string_view what);
  void on_accept(beast::error_code ec);
  void on_read(beast::error_code ec, std::size_t bytes);
  void on_write(beast::error_code ec, std::size_t bytes);

  void do_read();

public:
  Client(tcp::socket &&socket);
  ~Client();

  void run();
};

#endif
