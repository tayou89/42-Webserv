#pragma once

#include <map>
#include "server.hpp"

class EventLoop {
	public:
		EventLoop();
		virtual ~EventLoop();
		void EventSet();
		void run();

	private:
		EventLoop(const EventLoop &);
		EventLoop &operator=(const EventLoop &);
		std::map<int, Server *> _serverList; // _serverList[EVENT]
};