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
	int connection_socket = -1;
	char buffer[30000];
	std::unique_ptr<request> request;
};

struct HttpResponse {
    int status_code = 200;
    std::string status_text = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    // Builder-style setters (return *this for chaining)
    HttpResponse& set_status(int code, std::string text) {
        status_code = code;
        status_text = text;
        return *this;
    }

    HttpResponse& set_body(std::string content) {
        body = content;
        return *this;
    }

    HttpResponse& add_header(std::string key, std::string value) {
        headers[key] = value;
        return *this;
    }

    HttpResponse& set_content_type(std::string type) {
        headers["Content-Type"] = type;
        return *this;
    }

    // Serialize to HTTP wire format
    std::string to_string() const {
        std::string response = "HTTP/1.1 " + std::to_string(status_code) + " " + status_text + "\r\n";
        
        // Auto-add Content-Length if not already set
        if (headers.find("Content-Length") == headers.end() && !body.empty()) {
            response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
        }
        
        // Add all headers
        for (const auto& [key, value] : headers) {
            response += key + ": " + value + "\r\n";
        }
        
        response += "\r\n" + body;
        return response;
    }

    // Static factory methods for common responses
    static HttpResponse ok(std::string body_text) {
        HttpResponse r;
        r.set_body(body_text).set_content_type("text/plain");
        return r;
    }

    static HttpResponse html(std::string html_content) {
        HttpResponse r;
        r.set_body(html_content).set_content_type("text/html");
        return r;
    }

    static HttpResponse error_400(std::string message = "Bad Request") {
        HttpResponse r;
        r.set_status(400, "Bad Request").set_body(message).set_content_type("text/plain");
        return r;
    }

    static HttpResponse error_404(std::string message = "Not Found") {
        HttpResponse r;
        r.set_status(404, "Not Found").set_body(message).set_content_type("text/plain");
        return r;
    }
};

class SingleFileHttpServer {
public:
	SingleFileHttpServer(int port);
	~SingleFileHttpServer();
	
    struct simple_socket *create_socket(int domain, int service, int protocol, int port, u_long interface);
    void create_listening_socket(int domain, int service, int protocol, int port, u_long, int backlog);
    int connect_to_network(struct simple_socket *socket);
    void test_connection(int, std::string item_name);
	int start_listening();

	//Server stuff
	void launch_server();
	bool accepter();
    bool server_connection_handler(std::unique_ptr<serverObject> childServerObject);
    void writer(std::unique_ptr<serverObject> &childServerObject, const HttpResponse& responsesss);
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
