/**
 * @file   SimpleServer.hpp
 * @brief  Declares the SimpleServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef SimpleServer_hpp
#define SimpleServer_hpp

#include <stdio.h>
#include <unistd.h>
#include "../hdelibc.hpp"

namespace HDE {

class SimpleServer {

public:
	SimpleServer(int domain, int service, int protocol, int port, u_long interface, int backlog);
	virtual ~SimpleServer();

	virtual void launch()=0;

	//getters
	ListeningSocket * get_socket();


private:
	ListeningSocket * socket;
	virtual bool accepter() =0;
	virtual void handler() =0;
	virtual void responder()=0;
};

}  // namespace HDE

#endif
