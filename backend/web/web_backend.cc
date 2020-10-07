#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string_view>

namespace beast = boost::beast;
namespace ip = boost::asio::ip;
namespace net = boost::asio;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

void do_session(tcp::socket socket) {
  try {
    websocket::stream<beast::tcp_stream> ws(std::move(socket));
    ws.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));
    ws.set_option(
        websocket::stream_base::decorator([](websocket::response_type &res) {
          res.set(beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) +
                                           " websocket-server-coro");
        }));
    ws.accept();
    ws.write(net::buffer("Hello, World!"));

    for (;;) {
      beast::flat_buffer buffer;
      ws.read(buffer);

      ws.text(ws.got_text());
      ws.write(buffer.data());
    }
  } catch (beast::system_error const &se) {
    if (se.code() != websocket::error::closed) {
      std::cerr << "Error: " << se.code().message() << std::endl;
    }
  }
}

int main() {
  std::cout << "Hello, World!" << std::endl;
  net::io_context ioc;
  beast::tcp_stream sock(ioc);

  ip::tcp::acceptor acceptor(ioc);
  ip::tcp::endpoint endpoint(ip::tcp::v4(), 8000);
  acceptor.open(endpoint.protocol());
  acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
  acceptor.bind(endpoint);
  acceptor.listen();

  for (;;) {
    tcp::socket socket{ioc};
    acceptor.accept(socket);

    std::thread(&do_session, std::move(socket)).detach();
  }
}
