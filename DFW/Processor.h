#pragma once
#include <boost/program_options.hpp>
#include "Packet.h"

namespace opt = boost::program_options;
using namespace std;

enum Protocol
{
	PROTOCOL_ALL,
	PROTOCOL_TCP,
	PROTOCOL_UDP,
	PROTOCOL_ICMP
};
const string protocol_str[] = {
	"", "tcp", "udp", "icmp"
};

// ��� ó��, ȭ�鿡 ���
class Processor
{
private:
	static bool request(Packet& req, Packet& res);

public:
	static bool list();
	static bool accept_port(const string& port, const string& source, bool is_delete);
	static bool redirect(const string& port, const string& source, int to_port, bool is_delete);
	static void reload();
	static bool iptables(const string& rule, bool is_delete);
};

