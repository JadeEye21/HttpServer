#ifndef SimpleSocket_hpp
#define SimpleSocket_hpp



#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

namespace HDE{
	class SimpleSocket{
	private:
		int sock;
		struct sockaddr_in address;
		int connection;
	public:
	 SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
	 virtual int connect_to_network(int sock, struct sockaddr_in address) = 0;
	 void test_connection(int);

	 //Getter function
	 struct sockaddr_in get_address();
	 int get_sock();
	 int get_connection();

	};
}

#endif