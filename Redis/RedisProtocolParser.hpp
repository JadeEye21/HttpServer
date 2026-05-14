/**
 * @file   RedisProtocolParser.hpp
 * @brief  Declares the RedisProtocolParser class (public interface).
 * @author Harshal Dhagale
 */

#ifndef RedisProtocolParser_hpp
#define RedisProtocolParser_hpp

#include <iostream>
#include <string>

namespace hdcpp {

class RedisProtocolParser {
public:
	RedisProtocolParser();
	static bool input_type(std::string input, int& pointer);

	static bool parse_integer(std::string input, int& pointer);
	static bool parse_bulk_string(std::string input, int& pointer);
	static bool parse_array(std::string input, int& pointer);
	static bool parse_simple_string(std::string input, int& pointer);
	static bool parse_error(std::string input, int& pointer);
	static bool parse_null(std::string input, int& pointer);
	static bool parse_bool(std::string input, int& pointer);
	static bool parse_double(std::string input, int& pointer);

	/** Scan to first CRLF; on success pointer sits on '\\r', line_start is line begin. */
	static bool read_data(const std::string& input, int& pointer, int& line_start);

private:
};

}  // namespace hdcpp

#endif
