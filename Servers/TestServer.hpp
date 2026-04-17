/**
 * @file   TestServer.hpp
 * @brief  Declares the TestServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef TestServer_hpp
#define TestServer_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "SimpleServer.hpp"

namespace HDE {

class TestServer: public SimpleServer {
public:
	TestServer();
	void launch();

private:
	char buffer[30000];
	std::string referrer;
	std::string first_line;
	std::vector<std::string> v;
	std::unordered_map<std::string, std::string> data;
	int new_socket;

	void accepter();
	void handler();
	void responder();
	void reader();

    void read_first_line();
};

}  // namespace HDE

#endif
