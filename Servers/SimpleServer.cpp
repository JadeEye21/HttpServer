/**
 * @file   SimpleServer.cpp
 * @brief  Defines the SimpleServer class.
 * @author Harshal Dhagale
 */

#include "SimpleServer.hpp"

HDE::SimpleServer::SimpleServer(int domain, int service, int protocol, int port, u_long interface, int backlog) {
    socket = new ListeningSocket(domain, service, protocol, port, interface, backlog);
}

HDE::SimpleServer::~SimpleServer() {
    delete socket;
    socket = nullptr;
}

HDE::ListeningSocket * HDE::SimpleServer::get_socket(){
    return socket;
}

bool HDE::SimpleServer::accepter(){
    return false;
}