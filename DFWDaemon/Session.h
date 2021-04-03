#pragma once

#include "Include.h"
#include "FirewallSocket.h"

class Session :
	public enable_shared_from_this<Session>
{
private:
	shared_ptr<FirewallSocket> sock;
	Packet recv_packet;
	Packet packet;
	asio::io_context& context;

public:
	shared_ptr<Session> this_shared;

public:
	Session(asio::io_context& context, shared_ptr<Socket> stream);
	~Session();
	void run();

private:
	void close();
	void init();
	void handle_client();

private:
	void on_read_packet(bool is_success);

	void on_rule_list();
	void on_allow();
	void on_reload();
	void on_redirect();
	void on_iptables();
};