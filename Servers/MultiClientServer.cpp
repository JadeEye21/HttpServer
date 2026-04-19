/**
 * @file   MultiClientServer.cpp
 * @brief  Defines the MultiClientServer class.
 * @author Harshal Dhagale
 */

#include "MultiClientServer.hpp"
#include <chrono>
#include <cstring>
#include <unistd.h>


HDE::MultiClientServer::MultiClientServer():SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10) {
    launch();
}

void HDE::MultiClientServer::launch(){
    while(true){
        std::cout<<"============Waiting===============\n";

        accepter();
        
        std::cout<<"=============DONE==================\n";
    }
}

void HDE::MultiClientServer::handler(){
    std::cout<<"Handler function called\n";
}

void HDE::MultiClientServer::responder(){
    std::cout<<"Responder function called\n";
}

bool HDE::MultiClientServer::accepter(){
    std::unique_ptr<serverObject> childServerObject = std::make_unique<serverObject>();
    int *new_socket = &childServerObject->new_socket;
    struct sockaddr_in peer_address{};
    socklen_t addrlen = sizeof(peer_address);

    *new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&peer_address, &addrlen);
    if (*new_socket < 0) {
        perror("accept");
        return false;
    }

    std::thread child_thread(&HDE::MultiClientServer::handler_threaded, this, std::move(childServerObject));
    // child_threads.push_back(std::move(child_thread));
    child_thread.detach();
    return true;
}

bool HDE::MultiClientServer::handler_threaded(std::unique_ptr<serverObject> childServerObject){
    std::cout<<"Thread id :"<<std::this_thread::get_id()<<std::endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int *new_socket = &childServerObject->new_socket;
    char *buffer = childServerObject->buffer;

    std::memset(buffer, 0, 30000);
    const size_t max_read = 30000 - 1;
    ssize_t n = read(*new_socket, buffer, max_read);
    if (n < 0) {
        perror("read");
        close(*new_socket);
        *new_socket = -1;
        return false;
    }
    if (n == 0) {
        close(*new_socket);
        *new_socket = -1;
        return false;
    }
    buffer[static_cast<size_t>(n)] = '\0';

    HDE::MultiClientServer::read_first_line(childServerObject);
    HDE::MultiClientServer::verify(childServerObject);
    sleep(60);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;

    std::cout<<"Thread id :"<<std::this_thread::get_id();
    return true;

}

void HDE::MultiClientServer::read_first_line(std::unique_ptr<serverObject>& childServerObject){
    std::string curr;
    char *buffer = childServerObject->buffer;
    std::string *first_line = &childServerObject->first_line;
    for(int i=0;i<30000;i++){
        if(buffer[i] == (char)0 || buffer[i]=='\n'){
            break;
        }
        curr+=buffer[i];
    }
    *first_line = curr;
    // HTTP line ends with \r\n; we stop at \n so \r is still in curr — strip it for parsing.
    if (!(*first_line).empty() && (*first_line).back() == '\r') {
        (*first_line).pop_back();
    }
    childServerObject->url.reset(utl::Utils::parse_url(*first_line));
}

// void HDE::MultiClientServer::responder(std::unique_ptr<serverObject>& childServerObject){
//     std::string message;

// 	struct utl::URL* url = &childServerObject->url;

// 	std::cout << url->request_method << " " << url->requestor << std::endl;
//     message = "HTTP/1.1 200 OK\r\n\r\nRequested path: " + url->requestor + "\r\n";
// 	writer(message);
// }

void HDE::MultiClientServer::verify(std::unique_ptr<serverObject>& childServerObject){
    auto url = &childServerObject->url;
    if (!*url) {
        return;
    }
    if((*url)->requestor != "/" && (*url)->requestor != "/index.html"){
        std::cout<<"Throwing invalid, wrong path\n";
        writer(childServerObject, header_failure);
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
            writer(childServerObject, header_failure);
            return;
        }
        int content_length = content.size();
        std::cout<<content<<std::endl;
        index_html.close();
        std::string message = header_success_html + std::to_string(content_length) + "\r\n\r\n"+ content;
        writer(childServerObject, message);
    }
}

void HDE::MultiClientServer::writer(std::unique_ptr<serverObject>& childServerObject,std::string message){
    if (childServerObject->new_socket < 0) {
        childServerObject->url.reset();
        return;
    }
    const char* p = message.c_str();
    size_t remaining = message.length();
    while (remaining > 0) {
        ssize_t w = write(childServerObject->new_socket, p, remaining);
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
    close(childServerObject->new_socket);
    childServerObject->new_socket = -1;
    childServerObject->url.reset();
}