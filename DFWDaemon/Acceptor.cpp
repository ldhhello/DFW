#include "Acceptor.h"
#include "Session.h"

Acceptor::Acceptor(asio::io_context& ios, const string& path) :
	m_ios(ios),
	m_acceptor(m_ios,
		asio::local::stream_protocol::endpoint(
			path
		))
{
	::chmod(path.c_str(), 0700);

	m_acceptor.listen();
	shared_ptr<Socket> sock = make_shared<Socket>(context[0]);
	m_acceptor.async_accept(*sock, bind(&Acceptor::on_accept, this, sock));
}

void Acceptor::on_accept(shared_ptr<Socket> sock)
{
	try
	{
		auto session = make_shared<Session>(context[i], sock);
		session->this_shared = session;
		session->run();
	}
	catch (...) {}

	try
	{
		i++;
		if (i == THREAD_COUNT)
			i = 0;

		shared_ptr<Socket> new_sock = make_shared<Socket>(context[i]);
		m_acceptor.async_accept(*new_sock, bind(&Acceptor::on_accept, this, new_sock));
	}
	catch (exception& e)
	{
		cout << "Exception! " << e.what() << endl;
	}
}
