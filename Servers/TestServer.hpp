/**
 * @file   TestServer.hpp
 * @brief  Declares the TestServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef TestServer_hpp
#define TestServer_hpp

#include <stdio.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "SimpleServer.hpp"
#include "Utilities/Utils.hpp"

namespace HDE {

class TestServer: public SimpleServer {
public:
	TestServer();
	~TestServer() override = default;
    void verify();
    void launch() override;

	std::string header_success_text = "HTTP/1.1 200 OK\r\n\r\n";
	std::string header_success_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html; Content-Length:";
	std::string header_failure = "HTTP/1.1 400 Invalid\r\n\r\n";

	void responder() override;
    void writer(std::string message);
    void reader();

    void read_first_line();

private:
	char buffer[30000];
	std::string referrer;
	std::string first_line;
	std::vector<std::string> v;
	std::unordered_map<std::string, std::string> data;

	std::unique_ptr<utl::URL> url;
	int new_socket = -1;

	bool accepter() override;
	void handler() override;
	
};

}  // namespace HDE

#endif
