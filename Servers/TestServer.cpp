/**
 * @file   TestServer.cpp
 * @brief  Defines the TestServer class.
 * @author Harshal Dhagale
 */

#include "TestServer.hpp"
#include "Utilities/Utils.hpp"

#include <iostream>
#include <unistd.h>

#include <vector>

HDE::TestServer::TestServer():SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10){
	launch();
}

void HDE::TestServer::accepter(){
    struct sockaddr_in address = get_socket()->get_address();
    int addrlen = sizeof(address);
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);

    read(new_socket, buffer, 30000);

}

void HDE::TestServer::handler(){
    std::cout<< buffer << std::endl;
}

void HDE::TestServer::reader(){
    
    std::string value, key;
    bool key_complete = false;
    int count =0;

    for(int i=0;i<30000;i++){
        if(buffer[i] == (char)0){
            break;
        }
        if((char)buffer[i] == '\n'){
            data[key] = value;
            key = value = "";
            key_complete = false;
        }
        else{
            if(buffer[i]==':'){
                key_complete = true;
            }
            if(!key_complete) key += buffer[i];
            else value += buffer[i];
        }
    }
}

void HDE::TestServer::read_first_line(){
    std::string curr;
    for(int i=0;i<30000;i++){
        if(buffer[i] == (char)0 || buffer[i]=='\n'){
            break;
        }
        curr+=buffer[i];
    }
    first_line = curr;
    // HTTP line ends with \r\n; we stop at \n so \r is still in curr — strip it for parsing.
    if (!first_line.empty() && first_line.back() == '\r') {
        first_line.pop_back();
    }
}


void HDE::TestServer::responder() {
	std::string message;
	struct utl::URL* url = utl::Utils::parse_url(first_line);
	// Browsers only render the body (after the blank line). The status line is not shown as page text.
	message = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\n\r\n")
		+ "Method: " + url->request_method + "\n"
		+ "Requested path: " + url->requestor + "\n"
		+ "Client HTTP version: " + url->version + "\n";
	std::cout << url->request_method << " " << url->requestor << std::endl;
	write(new_socket, message.c_str(), message.length());
	close(new_socket);
	delete url;
}

void HDE::TestServer::launch(){
    while(true){
        std::cout<<"============Waiting===============\n";

        accepter();
        handler();
        reader();
        read_first_line();
        responder();

        std::cout<<"=============DONE==================\n";
    }
}