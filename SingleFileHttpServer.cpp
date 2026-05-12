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


hdcpp::SingleFileHttpServer::SingleFileHttpServer(int port) {
	create_listening_socket(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10);
	launch_server();
}

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
    hdcpp::simple_socket* connection_socket = new hdcpp::simple_socket();
    connection_socket->address.sin_family = domain;
    connection_socket->address.sin_port = htons(port);
    connection_socket->address.sin_addr.s_addr = htonl(interface);

    connection_socket->sock = socket(domain, service, protocol);
    test_connection(connection_socket->sock, "");
    return connection_socket;
}

void hdcpp::SingleFileHttpServer::create_listening_socket(int domain, int service, int protocol, int port, u_long interface, int backlog){
	this->listening_socket = create_socket(domain, service, protocol, port, interface);
	this->backlog = backlog;
	int binding = connect_to_network(this->listening_socket);
	if(binding < 0){
		perror("Socket failed to bind to the server");
		return;
	}
	test_connection(start_listening(), "started listening");
}

int hdcpp::SingleFileHttpServer::start_listening(){
	return listen(listening_socket->sock, backlog);
}

void hdcpp::SingleFileHttpServer::test_connection(int item_to_test, std::string item_name){
	//Confirms that the socket or connection has been properly established
	if(item_to_test <0){
		perror("Failed to connect...");
		exit(EXIT_FAILURE);
	}
    else{
        std::cout<<"Connected to the socket: "<<item_name<<" \n";
    }
}

int hdcpp::SingleFileHttpServer::connect_to_network(hdcpp::simple_socket* socket){
	int binding = bind(socket->sock, (struct sockaddr*)&(socket->address), sizeof(socket->address));
	test_connection(binding, "binding socket at connect_to_network");
	return binding;
}

void hdcpp::SingleFileHttpServer::launch_server(){
	while(true){
		std::cout<<"============Waiting===============\n";
		accepter();
        std::cout<<"=============DONE==================\n";
	}
}

bool hdcpp::SingleFileHttpServer::accepter(){
	std::unique_ptr<serverObject> childServerObject = std::make_unique<serverObject>();
	int *new_connecting_socket = &childServerObject->connection_socket;
	struct sockaddr_in peer_address{};
    socklen_t addrlen = sizeof(peer_address);
	*new_connecting_socket = accept(this->listening_socket->sock, (struct sockaddr *)&peer_address, &addrlen);

	if (*new_connecting_socket < 0) {
        perror("accept");
        return false;
    }

	std::thread child_thread(&SingleFileHttpServer::server_connection_handler, this, std::move(childServerObject));
	child_thread.detach();
	
	return true;
}

bool hdcpp::SingleFileHttpServer::server_connection_handler(std::unique_ptr<serverObject> childServerObject){
	std::cout<<"Thread id :"<<std::this_thread::get_id()<<std::endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	char *buffer = childServerObject->buffer;
	std::memset(buffer, 0, buffer_size);
	const size_t max_read = buffer_size -1;
	ssize_t n = read(childServerObject->connection_socket, buffer, max_read);

	if(n<=0){
		if(n<0)perror("Server not able to read the message");
		close(childServerObject->connection_socket);
		childServerObject.release();
		return false;
	}
    buffer[static_cast<size_t>(n)] = '\0';
	childServerObject->request = std::make_unique<hdcpp::request>();
	hdcpp::HttpParser::parse_request(childServerObject);
	auto response = hdcpp::HttpResponse::ok("Hello, " + childServerObject->request->request_path + " from hdcpp.\n");
	hdcpp::SingleFileHttpServer::writer(childServerObject, response);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;
    std::cout<<"Thread id :"<<std::this_thread::get_id();
	std::cout<<std::endl;
	return true;
}

void hdcpp::SingleFileHttpServer::writer(std::unique_ptr<serverObject>& childServerObject, const HttpResponse& response){
	if (childServerObject->connection_socket < 0) {
        childServerObject->request.reset();
        return;
    }
	std::string response_str = response.to_string();
    const char* p = response_str.c_str();
    size_t remaining = response_str.length();
    
    while(remaining > 0){
        ssize_t w = write(childServerObject->connection_socket, p, remaining);
        if(w < 0){
            perror("Server not able to write the message");
            break;
        }
        if(w == 0) break;
        p += static_cast<size_t>(w);
        remaining -= static_cast<size_t>(w);
    }

	close(childServerObject->connection_socket);
	childServerObject.release();
}

hdcpp::SingleFileHttpServer::~SingleFileHttpServer(){
	close(listening_socket->sock);
	delete(listening_socket);
}

