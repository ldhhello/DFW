#include "Firewall.h"
#include <iostream>

void Firewall::iptables(const string& str)
{
	system(("iptables " + str).c_str());
}

void Firewall::iptables(format f)
{
	iptables(f.str());
}

void Firewall::initialize()
{
	cout << "Initializing iptables rules..." << endl;

	// �̰� Docker����� �ϴ� �ֵ��̶� �� ������ ����. �׷��� �ϴ� �����ϰ� �� ����)
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

	//iptables("-A FIREWALL_INPUT -p tcp --dport 22 -j ACCEPT"); // �̰� ����� ���ҷ��� �ִ°���

	cout << "Successfully initialized!" << endl;
}

void Firewall::allow(int protocol, int port, const string& source)
{
	if (protocol == 0) // ��� ���������� ���⼭�� ��Ʈ ������ �ؾ� �ؼ� TCP / UDP�� ������.
	{
		allow(1, port, source); // TCP
		allow(2, port, source); // UDP
		return;
	}

	// ��� ��������, TCP, UDP, ICMP
	if (protocol > 4 || protocol < 0)
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str;
	if (protocol == 3 || port == 0) // icmp�̰ų� ��� ��Ʈ ����� ���
		port_str = "";
	else
		port_str = "--dport " + to_string(port);

	iptables(format("-A FIREWALL_INPUT %s %s %s -j ACCEPT") % protocol_str[protocol] % source_str % port_str);
}

void Firewall::redirect(int protocol, int port, const string& source, int to_port)
{
	if (protocol == 0) // ��� ���������� ���⼭�� ��Ʈ ������ �ؾ� �ؼ� TCP / UDP�� ������.
	{
		redirect(1, port, source, to_port); // TCP
		redirect(2, port, source, to_port); // UDP
		return;
	}

	// ��� ��������, TCP, UDP, ICMP
	if (protocol > 3 || protocol < 0) // ICMP�� ��Ʈ ������ ��� ����
		return;

	string source_str;
	if (source == "all")
		source_str = "";
	else
		source_str = "-s " + source;

	string port_str = "--dport " + to_string(port);

	iptables(format("-t nat -A FIREWALL_PREROUTING %s %s %s -j REDIRECT --to %d") % protocol_str[protocol] % source_str % port_str % to_port);
}