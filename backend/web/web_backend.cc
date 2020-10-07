#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ip = boost::asio::ip;

using namespace std;
using namespace beast;
using namespace beast::websocket;

int main() {
  std::cout << "Hello, World!" << std::endl;
  net::io_context ioc;
  tcp_stream sock(ioc);

  ip::tcp::acceptor acceptor(ioc);
  ip::tcp::endpoint endpoint(ip::tcp::v4(), 8000);
  acceptor.open(endpoint.protocol());
  acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
  acceptor.bind(endpoint);
  acceptor.listen();

  while (1) {
    stream<tcp_stream> ws(acceptor.accept());
    ws.accept();

    net::const_buffer b("Hello, World!", 13);
    ws.text(true);
    ws.write(b);

    while(1) {
      flat_buffer buffer;
      beast::error_code ec;
      ws.read(buffer, ec);
      if (ec == beast::websocket::error::closed) {
        break;
      }
      ws.text(ws.got_text());
      ws.write(buffer.data());
    }
  }
}
