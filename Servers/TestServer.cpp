/**
 * @file   TestServer.cpp
 * @brief  Defines the TestServer class.
 * @author Harshal Dhagale
 */

#include "TestServer.hpp"
#include "Utilities/Utils.hpp"
#include <cstring>
#include <fstream>

#include <iostream>
#include <unistd.h>

#include <vector>

HDE::TestServer::TestServer():SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10){
	launch();
}

bool HDE::TestServer::accepter(){
    struct sockaddr_in peer_address{};
    socklen_t addrlen = sizeof(peer_address);
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&peer_address, &addrlen);
    if (new_socket < 0) {
        perror("accept");
        return false;
    }

    std::memset(buffer, 0, sizeof(buffer));
    const size_t max_read = sizeof(buffer) - 1;
    ssize_t n = read(new_socket, buffer, max_read);
    if (n < 0) {
        perror("read");
        close(new_socket);
        new_socket = -1;
        return false;
    }
    if (n == 0) {
        close(new_socket);
        new_socket = -1;
        return false;
    }
    buffer[static_cast<size_t>(n)] = '\0';
    return true;
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
    url.reset(utl::Utils::parse_url(first_line));
}


void HDE::TestServer::responder() {
	std::string message;
	// struct utl::URL* url = utl::Utils::parse_url(first_line);
	// Browsers only render the body (after the blank line). The status line is not shown as page text.
	// message = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\n\r\n")
	// 	+ "Method: " + url->request_method + "\n"
	// 	+ "Requested path: " + url->requestor + "\n"
	// 	+ "Client HTTP version: " + url->version + "\n";
	std::cout << url->request_method << " " << url->requestor << std::endl;
    // message =url->version +" 200 OK\r\nRequested path: " + url->requestor+ "\r\n" ;
    message = "HTTP/1.1 200 OK\r\n\r\nRequested path: " + url->requestor + "\r\n";
	writer(message);
}

void HDE::TestServer::writer(std::string message){
    if (new_socket < 0) {
        url.reset();
        return;
    }
    const char* p = message.c_str();
    size_t remaining = message.length();
    while (remaining > 0) {
        ssize_t w = write(new_socket, p, remaining);
        if (w < 0) {
            perror("write");
            break;
        }
        if (w == 0) {
            break;
        }
        p += static_cast<size_t>(w);
        remaining -= static_cast<size_t>(w);
    }
    close(new_socket);
    new_socket = -1;
    url.reset();
}

void HDE::TestServer::verify(){
    if (!url) {
        return;
    }
    if(url->requestor != "/" && url->requestor != "/index.html"){
        std::cout<<"Throwing invalid, wrong path\n";
        writer(header_failure);
        return;
    }
    else{
        // Relative to cwd: repo root has Servers/index.html; running from build/ needs ../Servers/
        static const char* const index_paths[] = {
            "Servers/index.html",
            "index.html",
            "../Servers/index.html",
        };
        std::ifstream index_html;
        for (const char* p : index_paths) {
            index_html.clear();
            index_html.open(p);
            if (index_html.is_open()) {
                break;
            }
        }
        std::string content;
        std::string temp;
        if(index_html.is_open()){
            while(getline(index_html, temp)){
                content+=(temp + (std::string)"\n");
            }
        }else{
            std::cerr << "Failed to open file"<<std::endl;
            writer(header_failure);
            return;
        }
        int content_length = content.size();
        std::cout<<content<<std::endl;
        index_html.close();
        std::string message = header_success_html + std::to_string(content_length) + "\r\n\r\n"+ content;
        writer(message);
    }

}
void HDE::TestServer::launch(){
    while(true){
        std::cout<<"============Waiting===============\n";

        if (!accepter()) {
            continue;
        }
        handler();
        reader();
        read_first_line();
        // responder();
        verify();

        std::cout<<"=============DONE==================\n";
    }
}
