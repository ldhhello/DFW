#include "Include.h"
#include "Server.h"
#include "Firewall.h"
#include "FirewallFile.h"

const char* socket_path = "/var/run/ldh-firewall.sock";

int main()
{
	//Firewall::initialize();

	FirewallFile::reload();

	unlink(socket_path);

	Server server;
	server.run(socket_path);
}