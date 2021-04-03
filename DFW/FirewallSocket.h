#pragma once

// Boost Library
#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
#include <boost/optional.hpp>
#include "Packet.h"

namespace asio = boost::asio;

using Socket = asio::local::stream_protocol::socket;
using Endpoint = asio::local::stream_protocol::endpoint;

class FirewallSocket
{
private:
	asio::io_context context;
	Socket sock;
	string buffer;

public:
	FirewallSocket();

	bool connect(Endpoint ep);
	void close();

	bool write_packet(Packet& packet);
	bool read_packet(Packet& packet);
};

