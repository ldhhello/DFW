#include "Firewall.h"
#include <iostream>

void Firewall::iptables(const string& str)
{
	system(("iptables " + str).c_str());
}

void Firewall::iptables(format f)
{
	cout << "Firewall::iptables(" << f << ");\n";
	iptables(f.str());
}

void Firewall::initialize()
{
	cout << "Initializing iptables rules..." << endl;

	// 이게 Docker라던지 하는 애들이랑 안 맞을거 같음. 그래도 일단 구현하고 할 예정)
	// iptables("-F");
	// iptables("-X");
	// iptables("-Z");
	// 
	// iptables("-t nat -F");
	// iptables("-t nat -X");
	// iptables("-t nat -Z");

	iptables("-N FIREWALL_INPUT");
	iptables("-F FIREWALL_INPUT");

	iptables("-D INPUT -j DROP");
	iptables("-D INPUT -j FIREWALL_INPUT");

	iptables("-I INPUT -j DROP");
	iptables("-I INPUT -j FIREWALL_INPUT");
	iptables("-I INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT");

	iptables("-t nat -N FIREWALL_PREROUTING");
	iptables("-t nat -F FIREWALL_PREROUTING");

	iptables("-t nat -D PREROUTING -j FIREWALL_PREROUTING");
	iptables("-t nat -I PREROUTING -j FIREWALL_PREROUTING");

	iptables("-t nat -N FIREWALL_POSTROUTING");
	iptables("-t nat -F FIREWALL_POSTROUTING");

	iptables("-t nat -D POSTROUTING -j FIREWALL_POSTROUTING");
	iptables("-t nat -I POSTROUTING -j FIREWALL_POSTROUTING");

	//iptables("-A FIREWALL_INPUT -p tcp --dport 22 -j ACCEPT"); // 이거 디버깅 편할려고 있는거임

	cout << "Successfully initialized!" << endl;
}

void Firewall::allow(int protocol, int port, const string& source)
{
	if (protocol == 0) // 모든 프로토콜은 여기서는 포트 지정을 해야 해서 TCP / UDP만 구현함.
	{
		allow(1, port, source); // TCP
		allow(2, port, source); // UDP
		return;
	}

	// 모든 프로토콜, TCP, UDP, ICMP
	if (protocol > 4 || protocol < 0)
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str;
	if (protocol == 3 || port == 0) // icmp이거나 모든 포트 허용일 경우
		port_str = "";
	else
		port_str = "--dport " + to_string(port);

	iptables(format("-A FIREWALL_INPUT %s %s %s -j ACCEPT") % protocol_str[protocol] % source_str % port_str);
}

void Firewall::redirect(int protocol, int port, const string& source, int to_port)
{
	if (protocol == 0) // 모든 프로토콜은 여기서는 포트 지정을 해야 해서 TCP / UDP만 구현함.
	{
		redirect(1, port, source, to_port); // TCP
		redirect(2, port, source, to_port); // UDP
		return;
	}

	// 모든 프로토콜, TCP, UDP, ICMP
	if (protocol > 3 || protocol < 0) // ICMP는 포트 개념이 없어서 제외
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str = "--dport " + to_string(port);

	iptables(format("-t nat -A FIREWALL_PREROUTING %s %s %s -j REDIRECT --to %d") % protocol_str[protocol] % source_str % port_str % to_port);
}

void Firewall::snat(int protocol, int port, const string& source, const string& to)
{
	if (protocol == 0) // 모든 프로토콜은 여기서는 포트 지정을 해야 해서 TCP / UDP만 구현함.
	{
		snat(1, port, source, to); // TCP
		snat(2, port, source, to); // UDP
		return;
	}

	// 모든 프로토콜, TCP, UDP, ICMP
	if (protocol > 4 || protocol < 0)
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str;
	if (protocol == 3 || port == 0) // icmp이거나 모든 포트 허용일 경우
		port_str = "";
	else
		port_str = "--dport " + to_string(port);

	

	iptables(format("-A FIREWALL_POSTROUTING -t nat %s %s %s -j SNAT --to-source %s")
		% protocol_str[protocol] % source_str % port_str % to);
}

void Firewall::dnat(int protocol, int port, const string& source, const string& to)
{
	if (protocol == 0) // 모든 프로토콜은 여기서는 포트 지정을 해야 해서 TCP / UDP만 구현함.
	{
		dnat(1, port, source, to); // TCP
		dnat(2, port, source, to); // UDP
		return;
	}

	// 모든 프로토콜, TCP, UDP, ICMP
	if (protocol > 4 || protocol < 0)
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str;
	if (protocol == 3 || port == 0) // icmp이거나 모든 포트 허용일 경우
		port_str = "";
	else
		port_str = "--dport " + to_string(port);



	iptables(format("-A FIREWALL_PREROUTING -t nat %s %s %s -j DNAT --to-destination %s")
		% protocol_str[protocol] % source_str % port_str % to);
}

void Firewall::masquerade(int protocol, int port, const string& source, const string& to)
{
	if (protocol == 0) // 모든 프로토콜은 여기서는 포트 지정을 해야 해서 TCP / UDP만 구현함.
	{
		masquerade(1, port, source, to); // TCP
		masquerade(2, port, source, to); // UDP
		return;
	}

	// 모든 프로토콜, TCP, UDP, ICMP
	if (protocol > 4 || protocol < 0)
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str;
	if (protocol == 3 || port == 0) // icmp이거나 모든 포트 허용일 경우
		port_str = "";
	else
		port_str = "--dport " + to_string(port);



	iptables(format("-A FIREWALL_POSTROUTING -t nat %s %s %s -j MASQUERADE")
		% protocol_str[protocol] % source_str % port_str);
}
