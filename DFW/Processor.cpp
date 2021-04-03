#include "Processor.h"
#include "FirewallSocket.h"
#include "Method.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

bool Processor::request(Packet& req, Packet& res)
{
	FirewallSocket sock;
	if (!sock.connect(Endpoint("/var/run/ldh-firewall.sock")))
		return false;

	if (!sock.write_packet(req))
		return false;

	if (!sock.read_packet(res))
		return false;

	sock.close();
	return true;
}

bool Processor::list()
{
	Packet req, res;
	req.set_method(METHOD_RULE_LIST);

	if (!request(req, res))
		return false;

	if (res.method == METHOD_ERROR)
	{
		cout << "An error was occured! Error : " << res.get_string(0) << endl;
		return false;
	}

	cout << endl << "----- Rule List -----" << endl << endl;

	for (int i = 0; i < res.size(); i++)
	{
		cout << res.get_string(i) << endl;
	}

	cout << endl;
	return true;
}

bool Processor::accept_port(const string& port, const string& source, bool is_delete)
{
	vector<string> port_vec;
	boost::split(port_vec, port, boost::is_any_of("/"));

	if (port_vec.size() > 2)
		return false;

	int protocol = PROTOCOL_ALL; // TCP + UDP

	if (port_vec.size() == 2)
	{
		if (port_vec[1] == "tcp")
			protocol = PROTOCOL_TCP;
		else if (port_vec[1] == "udp")
			protocol = PROTOCOL_UDP;
		else if (port_vec[1] == "icmp")
			protocol = PROTOCOL_ICMP;
	}

	Packet req, res;
	req.set_method(METHOD_ALLOW);
	req.push(protocol);
	req.push(atoi(port_vec[0].c_str()));
	req.push(source);
	req.push(is_delete);

	if (!request(req, res))
		return false;

	if (res.method == METHOD_ERROR)
	{
		cout << "An error was occured! Error : " << res.get_string(0) << endl;
		return false;
	}

	cout << "Rule successfully added!" << endl;
	return true;
}

bool Processor::redirect(const string& port, const string& source, int to_port, bool is_delete)
{
	vector<string> port_vec;
	boost::split(port_vec, port, boost::is_any_of("/"));

	if (port_vec.size() > 2)
		return false;

	int protocol = PROTOCOL_ALL; // TCP + UDP

	if (port_vec.size() == 2)
	{
		if (port_vec[1] == "tcp")
			protocol = PROTOCOL_TCP;
		else if (port_vec[1] == "udp")
			protocol = PROTOCOL_UDP;
		else if (port_vec[1] == "icmp")
			protocol = PROTOCOL_ICMP;
	}

	Packet req, res;
	req.set_method(METHOD_REDIRECT);

	req.push(protocol);
	req.push(atoi(port_vec[0].c_str()));
	req.push(source);
	req.push(to_port);
	req.push(is_delete);

	if (!request(req, res))
		return false;

	if (res.method == METHOD_ERROR)
	{
		cout << "An error was occured! Error : " << res.get_string(0) << endl;
		return false;
	}

	cout << "Rule successfully added!" << endl;
	return true;
}

void Processor::reload()
{
	Packet req, res;
	req.set_method(METHOD_RELOAD);

	if (!request(req, res))
		return;

	cout << "Daemon successfully reloaded!" << endl;
}

bool Processor::iptables(const string& rule, bool is_delete)
{
	Packet req, res;
	req.set_method(METHOD_IPTABLES);

	req.push(rule);
	req.push(is_delete);

	if (!request(req, res))
		return false;

	if (res.method == METHOD_ERROR)
	{
		cout << "An error was occured! Error : " << res.get_string(0) << endl;
		return false;
	}

	cout << "Rule successfully added!" << endl;
	return true;
}
