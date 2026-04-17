/**
 * @file   Utils.hpp
 * @brief  Declares the Utils class (public interface).
 * @author Harshal Dhagale
 */

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

namespace utl {

class Utils {
public:
	Utils();
	static struct URL*  parse_url(std::string url);

private:
	static const std::string local_addr;
};

struct URL {
	std::string request_method;
	std::string requestor;
	std::string version;
};

}  // namespace utl

#endif
