#ifndef NET_HH
#define NET_HH

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_NETWORK_ENABLE_HTTPS
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

#endif
