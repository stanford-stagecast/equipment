#ifndef CLIENT_HH
#define CLIENT_HH

#include <string_view>
#include <queue>

#include "net.hh"
#include "beast.hh"

class Dispatcher;

class Client : public std::enable_shared_from_this<Client> {
private:
  std::shared_ptr<ssl::context> ctx_;
  websocket::stream<ssl::stream<beast::tcp_stream>> ws_;
  beast::flat_buffer buffer_;
  std::queue<std::string> queue_;
  std::shared_ptr<Dispatcher> dispatcher_;
  bool write_in_progress_;

  void fail(beast::error_code ec, std::string_view what);
  void on_accept(beast::error_code ec);
  void on_read(beast::error_code ec, std::size_t bytes);
  void on_write(beast::error_code ec, std::size_t bytes);

  void do_read();
  void do_write();

public:
  Client(std::shared_ptr<ssl::context> ctx, tcp::socket &&socket, std::shared_ptr<Dispatcher> dispatcher);
  ~Client();

  void run();
  void do_update(std::string_view update);
};

#endif
