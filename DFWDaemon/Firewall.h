#pragma once
#include "Include.h"
#include <boost/format.hpp>

using boost::format;

enum Protocol
{
	PROTOCOL_ALL,
	PROTOCOL_TCP,
	PROTOCOL_UDP,
	PROTOCOL_ICMP
};
const string protocol_str[] = {
	"", "-p tcp", "-p udp", "-p icmp"
};

class Firewall
{
public:
	static void iptables(const string& str);
	static void iptables(format f);

public:
	static void initialize();
	static void allow(int protocol, int port, const string& source);
	static void redirect(int protocol, int port, const string& source, int to_port);
};

