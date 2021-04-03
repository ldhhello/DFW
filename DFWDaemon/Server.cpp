#include "Server.h"
#include "Acceptor.h"

// صباح الخير

asio::io_context context[THREAD_COUNT];
//asio::io_context room_context[THREAD_COUNT];

void Server::run_context(asio::io_context& c)
{
	// 스택오버플로우에서 이렇게하면 안디진댔음.
	auto work = make_shared<asio::io_service::work>(c);
	c.run();
}

void Server::run_thread(asio::io_context& c)
{
	thread th([](Server* s, asio::io_context& c) {
		s->run_context(c);

		}, this, ref(c));
	th.detach();
}

void Server::init()
{
}

void Server::run(const string& path)
{
	init();

	//for (int i = 0; i < THREAD_COUNT; i++)
		//run_thread(room_context[i]);

	Acceptor acc(m_context, path);

	for (int i = 0; i < THREAD_COUNT; i++)
		run_thread(context[i]);

	run_context(m_context);
}