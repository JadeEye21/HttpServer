/**
 * @file   RedisMainServer.cpp
 * @brief  Defines the RedisMainServer class.
 * @author Harshal Dhagale
 */

#include "RedisMainServer.hpp"
#include "RedisProtocolParser.hpp"

hdcpp::RedisMainServer::RedisMainServer() {

}

void hdcpp::RedisMainServer::launch(){

    // int res = hdcpp::RedisMainServer::read_input(input);
    // if(res<0){
    //     perror("Wrong input");
    // }
    // if(res==0){
    //     return;
    // }
    
}

int hdcpp::RedisMainServer::read_input(std::string input){
    if(input.size() < 0){
        return -1;
    }
    std::cout<<"This is the input recieved: "<<input<<std::endl;
    int pointer =0;
    hdcpp::RedisProtocolParser::input_type(input, pointer);
    return 1;
}

