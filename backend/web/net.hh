#ifndef NET_HH
#define NET_HH

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

#endif
