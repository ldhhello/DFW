#include "FirewallSocket.h"

FirewallSocket::FirewallSocket()
	: sock(context)
{
}

bool FirewallSocket::connect(Endpoint ep)
{
	try
	{
		sock.connect(ep);
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void FirewallSocket::close()
{
	sock.close();
}

bool FirewallSocket::write_packet(Packet& packet)
{
	return packet.write([this](const void* b, int s) -> bool {
		try
		{
			asio::write(sock, asio::buffer(b, s));
		}
		catch (...)
		{
			return false;
		}
		return true;
	});
}

bool FirewallSocket::read_packet(Packet& packet)
{
	try
	{
		int packet_len;
		asio::read(sock, asio::buffer(&packet_len, 4));

		packet_len = ntohl(packet_len);

		buffer.resize(packet_len-4);
		asio::read(sock, asio::buffer(&buffer[0], packet_len-4));

		if (!packet.from_string(buffer))
			throw 1;
	}
	catch (...)
	{
		return false;
	}
	return true;
}
