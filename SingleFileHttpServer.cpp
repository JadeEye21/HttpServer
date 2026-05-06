/**
 * @file   SingleFileHttpServer.cpp
 * @brief  Defines the SingleFileHttpServer class.
 * @author Harshal Dhagale
 */

#include "SingleFileHttpServer.hpp"
const std::string hdcpp::SingleFileHttpServer::local_addr = "http://localhost/";
const std::string header_success_text = "HTTP/1.1 200 OK\r\n\r\n";
const std::string header_success_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html; Content-Length:";
const std::string header_failure = "HTTP/1.1 400 Invalid\r\n\r\n";
const std::string CRLF = "\r\n";
const std::string DOUBLE_CRLF = "\r\n\r\n";
const int buffer_size = 30000;


hdcpp::SingleFileHttpServer::SingleFileHttpServer() {}

/*Http Parser functions, can be taken into a separate CPP file*/

void hdcpp::HttpParser::parse_request(std::unique_ptr<serverObject>& childServerObject) {
	auto buffer = childServerObject->buffer;

	int pointer = 0;
	int line_counter =0;
	int skip =0;
	while(pointer < buffer_size){
		bool found_delim = 0;
		if (pointer + 4 < buffer_size && buffer[pointer] == '\r') {
			if (std::string(buffer + pointer, 4) == DOUBLE_CRLF) {
				parse_request_body(childServerObject, std::string(buffer + pointer + 4, buffer_size - (pointer + 4)));
				found_delim = true;
				break;
			}
			else if (pointer +2 < buffer_size && std::string(buffer + pointer, 2) == CRLF) {
				if (line_counter == 0) {
					parse_first_line(childServerObject, std::string(buffer + skip, pointer - skip));
					line_counter++;
				}
				else {
					parse_request_headers(childServerObject, std::string(buffer + skip, pointer - skip));
				}
				pointer += 2;
				skip = pointer;
				found_delim=true;
			}
		}
		if(!found_delim)pointer++;
	}
}

void hdcpp::HttpParser::parse_first_line(std::unique_ptr<serverObject>& childServerObject, std::string line){
	size_t pointer =0;
	size_t start=0;
	while(pointer<line.size() && line[pointer] != ' '){
		pointer++;
	}
	childServerObject->request->request_method = line.substr(start, pointer - start);
	if (pointer < line.size())pointer++;
	start = pointer;
	while(pointer<line.size() && line[pointer] != ' '){
		pointer++;
	}
	childServerObject->request->request_path = line.substr(start, pointer - start);
	if (pointer < line.size())pointer++;
	start = pointer;
	while(pointer<line.size() && line[pointer] != ' '){
		pointer++;
	}
	childServerObject->request->request_version = line.substr(start, pointer - start);

	//The superior version;
	// std::stringstream ss(line);
	// ss >> childServerObject->request->request_method;
	// ss >> childServerObject->request->request_path;
	// ss >> childServerObject->request->request_version;
}

void hdcpp::HttpParser::parse_request_headers(std::unique_ptr<serverObject>& childServerObject, std::string line){
	size_t pointer =0;
	while(pointer<line.size() && line[pointer]!=':'){
		pointer++;
	}
	childServerObject->request->request_headers[line.substr(0, pointer)] = line.substr(pointer+2);
}

void hdcpp::HttpParser::parse_request_body(std::unique_ptr<serverObject>& childServerObject, std::string buffer) {
    if (childServerObject->request->request_headers.count("Content-Length")) {
        try {
            size_t max_size = std::stoul(childServerObject->request->request_headers["Content-Length"]);
            size_t actual_length = std::min(max_size, buffer.size());
            childServerObject->request->request_body = buffer.substr(0, actual_length);
        } catch (...) {
            childServerObject->request->request_body = "";
        }
    } else {
        childServerObject->request->request_body = buffer;
    }

    std::string& body = childServerObject->request->request_body;
    size_t first = body.find_first_not_of(" \t\n\r");
    if (first != std::string::npos) {
        size_t last = body.find_last_not_of(" \t\n\r");
        body = body.substr(first, (last - first + 1));
    } else {
        body.clear();
    }
}


/*Server functions*/
hdcpp::simple_socket* hdcpp::SingleFileHttpServer::create_socket(int domain, int service, int protocol, int port, u_long interface){
    hdcpp::simple_socket* new_socket = new hdcpp::simple_socket();
    new_socket->address.sin_family = domain;
    new_socket->address.sin_port = htons(port);
    new_socket->address.sin_addr.s_addr = htonl(interface);

    new_socket->sock = socket(domain, service, protocol);
    test_connection(new_socket->sock);
    return new_socket;
}

void hdcpp::SingleFileHttpServer::create_listening_socket(int domain, int service, int protocol, int port, u_long interface, int backlog){
	this->listening_socket = create_socket(domain, service, protocol, port, interface);
	this->backlog = backlog;
	test_connection(start_listening());
}

int hdcpp::SingleFileHttpServer::start_listening(){
	return listen(listening_socket->sock, backlog);
}

void hdcpp::SingleFileHttpServer::test_connection(int item_to_test){
	//Confirms that the socket or connection has been properly established
	if(item_to_test <0){
		perror("Failed to connect...");
		exit(EXIT_FAILURE);
	}
    else{
        std::cout<<"Connected to the socket\n";
    }
}

int hdcpp::SingleFileHttpServer::connect_to_network(hdcpp::simple_socket* socket){
	int binding = bind(socket->sock, (struct sockaddr*)&(socket->address), sizeof(socket->address));
	test_connection(binding);
	return binding;
}

void hdcpp::SingleFileHttpServer::launch_server(int domain, int service, int protocol, int port, u_long interface, int backlog){
	while(true){
		std::cout<<"============Waiting===============\n";
		accepter();
        std::cout<<"=============DONE==================\n";
	}
}

bool hdcpp::SingleFileHttpServer::accepter(){
	std::unique_ptr<serverObject> childServerObject = std::make_unique<serverObject>();
	int *new_connecting_socket = &childServerObject->new_socket;
	struct sockaddr_in peer_address{};
    socklen_t addrlen = sizeof(peer_address);
	*new_connecting_socket = accept(this->listening_socket->sock, (struct sockaddr *)&peer_address, &addrlen);

	if (*new_connecting_socket < 0) {
        perror("accept");
        return false;
    }

	std::thread child_thread(&server_connection_handler, this, std::move(childServerObject));
	child_thread.detach();
	
	return true;
}

bool hdcpp::SingleFileHttpServer::server_connection_handler(std::unique_ptr<serverObject> childServerObject){
	return true;
}

hdcpp::SingleFileHttpServer::~SingleFileHttpServer(){
	close(listening_socket->sock);
	delete(listening_socket);
}

