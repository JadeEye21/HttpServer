#include "hdelibc.hpp"
#include <stdio.h>
#include "Servers/TestServer.hpp"

int main(){
    // std::cout<<"Starting socket...."<<std::endl;
    // std::cout<<"Binding socket...."<<std::endl;
    // HDE::BindingSocket bs = HDE::BindingSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
    // std::cout<<"Listening socket...."<<std::endl;
    // HDE::ListeningSocket ls = HDE::ListeningSocket(AF_INET, SOCK_STREAM, 0, 81, INADDR_ANY, 10);
    // std::cout<<"Success \n";
    HDE::TestServer t;
}
