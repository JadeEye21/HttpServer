/**
 * @file   SingleFileHttpServer.hpp
 * @brief  Declares the SingleFileHttpServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef SingleFileHttpServer_hpp
#define SingleFileHttpServer_hpp

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>


namespace hdcpp {

struct request {
	std::string request_method;
	std::string request_path;
	std::string request_version;
	std::unordered_map<std::string, std::string> request_headers;
	std::string request_body;
};

struct simple_socket {
	int sock;
	struct sockaddr_in address;
	int connection;
};

struct serverObject{
	int new_socket = -1;
	char buffer[30000];
	std::unique_ptr<request> request;
};

class SingleFileHttpServer {
public:
	SingleFileHttpServer();
	~SingleFileHttpServer();
	
    struct simple_socket *create_socket(int domain, int service, int protocol, int port, u_long interface);
    void create_listening_socket(int domain, int service, int protocol, int port, u_long, int backlog);
    int connect_to_network(struct simple_socket *socket);
    void test_connection(int);
	int start_listening();

	//Server stuff
	void launch_server(int domain, int service, int protocol, int port, u_long interface, int backlog);
	bool accepter();
    bool server_connection_handler(std::unique_ptr<serverObject> childServerObject);
    void handler();

private:
	static const std::string local_addr;
	simple_socket* listening_socket;
	int backlog;
};

class HttpParser{
	public:
	static void parse_request(std::unique_ptr<serverObject>& childServerObject);
    static void parse_first_line(std::unique_ptr<serverObject> &childServerObject, std::string line);
    static void parse_request_headers(std::unique_ptr<serverObject> &childServerObject, std::string line);
    static void parse_request_body(std::unique_ptr<serverObject> &childServerObject, std::string buffer);
};


}  // namespace hdcpp


#endif
