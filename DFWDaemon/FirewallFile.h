#pragma once
#include <string>
#include <vector>
#include "Json.h"
using namespace std;

class FirewallFile
{
public:
	static void reload();

	static void allow(int protocol, int port, const string& source);
	static bool delete_allow(int protocol, int port, const string& source);

	static void redirect(int protocol, int port, const string& source, int to_port);
	static bool delete_redirect(int protocol, int port, const string& source, int to_port);

	static bool list(vector<string>& vec);

	static void iptables(const string& rule);
	static bool delete_iptables(const string& rule);

	static void snat(int protocol, int port, const string& source, const string& to);
	static bool delete_snat(int protocol, int port, const string& source, const string& to);
	static void dnat(int protocol, int port, const string& source, const string& to);
	static bool delete_dnat(int protocol, int port, const string& source, const string& to);
	static void masquerade(int protocol, int port, const string& source, const string& to);
	static bool delete_masquerade(int protocol, int port, const string& source, const string& to);
};

