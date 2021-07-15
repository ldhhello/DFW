#include "Session.h"
#include "Utils.h"
#include "Method.h"
#include "Firewall.h"
#include "FirewallFile.h"
#include <set>

Session::Session(asio::io_context& context, shared_ptr<Socket> stream) :
	sock(new FirewallSocket(context, stream)),
	context(context)
{
	sock->this_shared = sock;

	debug_log << "Session connected!" << endl;
}

Session::~Session()
{
	debug_log << "Session deleted!" << endl;
}

void Session::run()
{
	init();

	try
	{
		handle_client();
	}
	catch (...) {}
}

void Session::close()
{
	debug_log << "Session : closing..." << endl;

	sock.reset();
	this_shared.reset();
}

void Session::init()
{
}

void Session::handle_client()
{
	sock->async_read_packet(recv_packet, bind(&Session::on_read_packet, shared_from_this(), placeholders::_1));
}

void Session::on_read_packet(bool is_success)
{
	if (!is_success)
	{
		//cout << "Error on Session::on_read_packet" << endl;
		close();
		return;
	}

	switch (recv_packet.method)
	{
		HANDLE_METHOD(METHOD_RULE_LIST, on_rule_list, 0);
		HANDLE_METHOD(METHOD_ALLOW, on_allow, 4);
		HANDLE_METHOD(METHOD_RELOAD, on_reload, 0);
		HANDLE_METHOD(METHOD_REDIRECT, on_redirect, 5);
		HANDLE_METHOD(METHOD_IPTABLES, on_iptables, 2);
		HANDLE_METHOD(METHOD_SNAT, on_snat, 5);
		HANDLE_METHOD(METHOD_DNAT, on_dnat, 5);
		HANDLE_METHOD(METHOD_MASQUERADE, on_masquerade, 5);

	default:
		close();
		return;
	}

	sock->async_read_packet(recv_packet, bind(&Session::on_read_packet, shared_from_this(), placeholders::_1));
}

void Session::on_rule_list()
{
	packet.clear();
	packet.set_method(METHOD_OK);
	vector<string>& rules = packet.vec;

	if (!FirewallFile::list(rules))
	{
		packet.clear();
		packet.set_method(METHOD_ERROR);
		packet.push("Failed to list all rules.");
	}

	sock->write_packet(packet);
}

void Session::on_allow()
{
	bool is_delete = recv_packet.get_int(3);

	if (is_delete)
		FirewallFile::delete_allow(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2));
	else
		FirewallFile::allow(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2));

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}

void Session::on_reload()
{
	FirewallFile::reload();

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}

void Session::on_redirect()
{
	bool is_delete = recv_packet.get_int(4);

	if(is_delete)
		FirewallFile::delete_redirect(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_int(3));
	else
		FirewallFile::redirect(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_int(3));

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}

void Session::on_iptables()
{
	bool is_delete = recv_packet.get_int(1);

	if (is_delete)
		FirewallFile::delete_iptables(recv_packet.get_string(0));
	else
		FirewallFile::iptables(recv_packet.get_string(0));

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}

void Session::on_snat()
{
	bool is_delete = recv_packet.get_int(4);

	if (is_delete)
		FirewallFile::delete_snat(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_string(3));
	else
		FirewallFile::snat(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_string(3));

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}

void Session::on_dnat()
{
	bool is_delete = recv_packet.get_int(4);

	if (is_delete)
		FirewallFile::delete_dnat(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_string(3));
	else
		FirewallFile::dnat(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_string(3));

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}

void Session::on_masquerade()
{
	bool is_delete = recv_packet.get_int(4);

	if (is_delete)
		FirewallFile::delete_masquerade(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_string(3));
	else
		FirewallFile::masquerade(recv_packet.get_int(0), recv_packet.get_int(1), recv_packet.get_string(2), recv_packet.get_string(3));

	packet.clear();
	packet.set_method(METHOD_OK);
	sock->write_packet(packet);
}
