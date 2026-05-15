/**
 * @file   RedisProtocolSerializer.hpp
 * @brief  Declares the RedisProtocolSerializer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef RedisProtocolSerializer_hpp
#define RedisProtocolSerializer_hpp

#include <iostream>

namespace hdcpp {

class RedisProtocolSerializer {
public:
	RedisProtocolSerializer();
	static std::string read_input(std::string input);
private:
};

}  // namespace hdcpp

#endif
