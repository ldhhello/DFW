#pragma once
#include "Include.h"

class Acceptor
{
private:
	asio::io_context& m_ios;
	asio::local::stream_protocol::acceptor m_acceptor;
	//asio::ssl::context m_ssl_context;
	int i = 0;

public:
	Acceptor(asio::io_context& ios, const string& path);
	void on_accept(shared_ptr<Socket> sock);
};