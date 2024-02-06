#pragma once

#include <sys/event.h>
#include <vector>

class Server {
	public:
		Server(Config conf);
		virtual ~Server();

		int createClientSocket(); // accept new client
		struct kevent eventRegister(); // ev.newEvent(ev.sv[idx].eventRegister())
		void getEvent(struct kevent event); //_serverList[idx].getEvent(eventlist[j]);
		void deleteEvent(struct kevent event); //EV_DELETE register, _fds.delete(), clost(event.ident)
		void readEvent(int fd); // read socket descriptor when new event occurred, call protocol
		void excuteEvent(); // input something returned from protocol (instance or std::string or ...)
		void sendEvent(int fd, std::string content); // register new send Event. send content to fd.

	private:
		Server();
		Server(const Server &);
		Server &operator=(const Server &);

		Config _conf;
		int _listenSocket; // set on constructor
		std::vector<int> _fds;

		Protocol _protocol;
};