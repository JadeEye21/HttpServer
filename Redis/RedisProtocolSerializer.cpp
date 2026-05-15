/**
 * @file   RedisProtocolSerializer.cpp
 * @brief  Defines the RedisProtocolSerializer class.
 * @author Harshal Dhagale
 */

#include "RedisProtocolSerializer.hpp"

hdcpp::RedisProtocolSerializer::RedisProtocolSerializer() {}

std::string hdcpp::RedisProtocolSerializer::read_input(std::string input){
    std::string serialized_string="*";
    //The result will always be an array of bulk strings.
    std::vector<std::string> words;
    std::string temp;
    int index =0;
    bool in_quotes=false;
    while(index<input.size()){
        if(input[index] !=' '){
            if(input[index] == '\"'){
                in_quotes = !in_quotes;
            }
            else{
                temp += input[index];
            }
        }
        else{
            if(in_quotes){
                temp += input[index];
            }
            else{
                words.push_back(temp);
                temp.clear();
            }
        }
        index++;
    }
    if(!temp.empty()) words.push_back(temp);
    serialized_string += std::to_string(words.size()) + "\r\n";
    for(auto word : words){
        serialized_string += "$" + std::to_string(word.size()) + "\r\n" + word + "\r\n";
    }
    serialized_string += "\r\n";
    std::cout<<"Serialized string: "<<serialized_string<<std::endl;
    return serialized_string;
}
