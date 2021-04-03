#include "FirewallSocket.h"

FirewallSocket::FirewallSocket(asio::io_context& c, shared_ptr<Socket> s) :
	context(c),
	sock(s),
	deadline(context)
{
}

FirewallSocket::~FirewallSocket()
{
	close();
	debug_log << "Socket Deleted!" << endl;
}

void FirewallSocket::close(bool is_force)
{
	if (this == NULL)
		return;

	debug_log << "Socket Closed!" << endl;

	if (!is_force)
		IGNORE_EXCEPTION(sock->shutdown(asio::ip::tcp::socket::shutdown_both));

	IGNORE_EXCEPTION(sock->lowest_layer().close());

	if (!is_force)
		IGNORE_EXCEPTION(deadline.cancel());

	this_shared.reset();
}

bool FirewallSocket::write(const void* buf, int size)
{
	//lock_guard<recursive_mutex> guard(m);

	try
	{
		asio::write(*sock, asio::buffer(buf, size));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool FirewallSocket::write_packet(const Packet& packet)
{
	if (this == NULL)
		return false;

	lock_guard<recursive_mutex> guard(m);

	packet.to_string(buffer);

	if (!write(&buffer[0], buffer.size()))
		return false;

	//if (!packet.write(bind(&MafiaSocket::write, this, placeholders::_1, placeholders::_2)))
		//return false;

	return true;
}

void FirewallSocket::async_read_packet(Packet& packet, function<void(bool, Packet&)> callback)
{
	if (this == NULL)
		return;

	deadline.expires_from_now(boost::posix_time::seconds(15));

	deadline.async_wait(
		bind(&FirewallSocket::check_deadline, this, placeholders::_1));

	asio::async_read(*sock, asio::buffer(&packet_len, 4), [this, &packet, callback](const boost::system::error_code& ec, size_t read_sz) {

		if ((ec) || read_sz == 0)
		{
			if (ec)
			{
				//cout << "boost::system::error_code! Message: " << ec.message() << endl;
			}

			callback(false, packet);
			return;
		}

		packet_len = ntohl(packet_len);

		packet_str.clear();

		if (packet_len > 10000 || packet_len < 0)
		{
			cout << "Invalid packet size : " << packet_len << " bytes" << endl;

			callback(false, packet);
			return;
		}

		packet_str.resize(packet_len - 4);

		deadline.expires_from_now(boost::posix_time::seconds(15));

		deadline.async_wait(
			bind(&FirewallSocket::check_deadline, this, placeholders::_1));

		asio::async_read(*sock, asio::buffer(&packet_str[0], packet_len - 4),
			bind(&FirewallSocket::on_read_packet, this, ref(packet), callback, placeholders::_1, placeholders::_2));
		});
}

void FirewallSocket::on_read_packet(Packet& packet, function<void(bool, Packet&)> callback, const boost::system::error_code& ec, size_t read_sz)
{
	if ((ec) || read_sz == 0)
	{
		if (ec)
		{
			//cout << "boost::system::error_code! Message: " << ec.message() << endl;
		}

		callback(false, packet);
		return;
	}

	if (!packet.from_string(packet_str))
	{
		callback(false, packet);
		return;
	}

	callback(true, packet);
}

void FirewallSocket::check_deadline(const boost::system::error_code& error)
{
	if (error)
		return;

	try
	{
		if (!sock->lowest_layer().is_open())
			return;
	}
	catch (...) {}

	if (deadline.expires_at() > asio::deadline_timer::traits_type::now())
	{
		deadline.async_wait(
			bind(&FirewallSocket::check_deadline, this, placeholders::_1));
	}
	else
	{
		close(true);
	}
}
