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
	std::string header_success_text = "HTTP/1.1 200 OK\r\n\r\n";
	std::string header_success_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html; Content-Length:";
	std::string header_failure = "HTTP/1.1 400 Invalid\r\n\r\n";


private:
	int new_socket;

	ListeningSocket * socket;
	virtual bool accepter();
	virtual void handler() =0;
	virtual void responder()=0;
};

}  // namespace HDE

#endif
