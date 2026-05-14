/**
 * @file   RedisMainServer.hpp
 * @brief  Declares the RedisMainServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef RedisMainServer_hpp
#define RedisMainServer_hpp

#include <iostream>
#include "RedisProtocolParser.hpp"

namespace hdcpp {

class RedisMainServer {
public:
	RedisMainServer();

    void launch();
	int read_input(std::string input);

private:
};

}  // namespace hdcpp

#endif
