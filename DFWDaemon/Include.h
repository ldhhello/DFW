#pragma once

// STD Library
#include <iostream>

#include <string>
#include <thread>
#include <atomic>
#include <future>
#include <memory>
#include <queue>

using namespace std;

// Boost Library
#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
#include <boost/optional.hpp>

// Defines
using Port = unsigned short;
namespace asio = boost::asio;

//using SSLStream = asio::ssl::stream<asio::ip::tcp::socket>;
using Socket = asio::local::stream_protocol::socket;
using socket_type = asio::detail::socket_type;

const int THREAD_COUNT = 1;

extern asio::io_context context[THREAD_COUNT];
//extern asio::io_context room_context[THREAD_COUNT];

#define IGNORE_EXCEPTION(c) try { c; } catch(...) {}

#define CHECK_PACKET_SIZE(s)     \
if (recv_packet.size() != s)     \
{                                \
	close();                     \
	return;                      \
}

#define HANDLE_METHOD(method, function, size) \
case method:                                  \
	CHECK_PACKET_SIZE(size);                  \
	function();                               \
	break;

#define debug_log cout