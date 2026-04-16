/**
 * @file   TestServer.hpp
 * @brief  Declares the TestServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef TestServer_hpp
#define TestServer_hpp

#include <stdio.h>
#include "SimpleServer.hpp"

namespace HDE {

class TestServer: public SimpleServer {
public:
	TestServer();
	void launch();

private:
	char buffer[30000];
	int new_socket;

	void accepter();
	void handler();
	void responder();

};

}  // namespace HDE

#endif
