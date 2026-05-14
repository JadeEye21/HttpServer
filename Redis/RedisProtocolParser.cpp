/**
 * @file   RedisProtocolParser.cpp
 * @brief  Defines the RedisProtocolParser class.
 * @author Harshal Dhagale
 */

#include "RedisProtocolParser.hpp"

#include <charconv>
#include <cctype>
#include <cstdint>
#include <limits>
#include <string_view>

namespace {

bool all_digits(std::string_view s) {
	for (char c : s) {
		if (!std::isdigit(static_cast<unsigned char>(c))) {
			return false;
		}
	}
	return !s.empty();
}

bool parse_int64_token(std::string_view token, std::int64_t& out) {
	const char* first = token.data();
	const char* last = token.data() + token.size();
	auto r = std::from_chars(first, last, out);
	return r.ec == std::errc() && r.ptr == last;
}

bool parse_bulk_length_token(const std::string& token, int& out_n) {
	if (token == "-1") {
		out_n = -1;
		return true;
	}
	if (token.empty() || token[0] == '-') {
		return false;
	}
	if (!all_digits(std::string_view(token))) {
		return false;
	}
	long long v = 0;
	try {
		v = std::stoll(token);
	} catch (const std::exception&) {
		return false;
	}
	if (v < 0 || v > static_cast<long long>(std::numeric_limits<int>::max())) {
		return false;
	}
	out_n = static_cast<int>(v);
	return true;
}

}  // namespace

hdcpp::RedisProtocolParser::RedisProtocolParser() {}

bool hdcpp::RedisProtocolParser::read_data(const std::string& input, int& pointer, int& line_start) {
	line_start = pointer;
	const int n = static_cast<int>(input.size());
	while (pointer < n) {
		if (input[pointer] == '\r' && pointer + 1 < n && input[pointer + 1] == '\n') {
			break;
		}
		++pointer;
	}
	if (pointer + 1 >= n) {
		return false;
	}
	if (input[pointer] != '\r' || input[pointer + 1] != '\n') {
		return false;
	}
	return true;
}

bool hdcpp::RedisProtocolParser::input_type(std::string input, int& pointer) {
	if (pointer >= static_cast<int>(input.size())) {
		return false;
	}
	switch (input[pointer++]) {
		case '*':
			return parse_array(input, pointer);
		case '$':
			return parse_bulk_string(input, pointer);
		case ':':
			return parse_integer(input, pointer);
		case '+':
			return parse_simple_string(input, pointer);
		case '-':
			return parse_error(input, pointer);
		case '_':
			return parse_null(input, pointer);
		case '#':
			return parse_bool(input, pointer);
		case ',':
			return parse_double(input, pointer);
		default:
			return false;
	}
}

bool hdcpp::RedisProtocolParser::parse_bulk_string(std::string input, int& pointer) {
	int head = 0;
	if (!read_data(input, pointer, head)) {
		return false;
	}
	const std::string len_token = input.substr(head, pointer - head);
	int n = 0;
	if (!parse_bulk_length_token(len_token, n)) {
		return false;
	}
	pointer += 2;
	if (n == -1) {
		std::cout << "NULL" << std::endl;
		return true;
	}
	if (pointer + n + 2 > static_cast<int>(input.size())) {
		return false;
	}
	if (input[pointer + n] != '\r' || input[pointer + n + 1] != '\n') {
		return false;
	}
	const std::string result_string = input.substr(pointer, n);
	std::cout << "This is result string: " << result_string << std::endl;
	pointer += n + 2;
	return true;
}

bool hdcpp::RedisProtocolParser::parse_integer(std::string input, int& pointer) {
	int head = 0;
	if (!read_data(input, pointer, head)) {
		return false;
	}
	const std::string_view token(input.data() + head, pointer - head);
	std::int64_t v = 0;
	if (!parse_int64_token(token, v)) {
		return false;
	}
	std::cout << "This is result number: " << v << std::endl;
	pointer += 2;
	return true;
}

bool hdcpp::RedisProtocolParser::parse_array(std::string input, int& pointer) {
	int head = 0;
	if (!read_data(input, pointer, head)) {
		return false;
	}
	const std::string count_token = input.substr(head, pointer - head);
	int count = 0;
	if (count_token == "-1") {
		count = -1;
	} else if (!parse_bulk_length_token(count_token, count)) {
		return false;
	}
	pointer += 2;
	if (count == -1) {
		std::cout << "NULL array" << std::endl;
		return true;
	}
	if (count < 0) {
		return false;
	}
	std::cout << "\nSize of array: " << count << std::endl;
	for (int i = 0; i < count; ++i) {
		if (!input_type(input, pointer)) {
			return false;
		}
	}
	return true;
}

bool hdcpp::RedisProtocolParser::parse_simple_string(std::string input, int& pointer) {
	int head = 0;
	if (!read_data(input, pointer, head)) {
		return false;
	}
	std::cout << input.substr(head, pointer - head) << std::endl;
	pointer += 2;
	return true;
}

bool hdcpp::RedisProtocolParser::parse_error(std::string input, int& pointer) {
	return parse_simple_string(input, pointer);
}

bool hdcpp::RedisProtocolParser::parse_null(std::string input, int& pointer) {
	const int n = static_cast<int>(input.size());
	if (pointer + 1 >= n || input[pointer] != '\r' || input[pointer + 1] != '\n') {
		return false;
	}
	std::cout << "NULL" << std::endl;
	pointer += 2;
	return true;
}

bool hdcpp::RedisProtocolParser::parse_bool(std::string input, int& pointer) {
	int head = 0;
	if (!read_data(input, pointer, head)) {
		return false;
	}
	const std::string res = input.substr(head, pointer - head);
	if (res.size() != 1 || (res[0] != 't' && res[0] != 'f')) {
		return false;
	}
	std::cout << (res == "t" ? "true" : "false") << std::endl;
	pointer += 2;
	return true;
}

bool hdcpp::RedisProtocolParser::parse_double(std::string input, int& pointer) {
	int head = 0;
	if (!read_data(input, pointer, head)) {
		return false;
	}
	const std::string token = input.substr(head, pointer - head);
	try {
		const double d = std::stod(token);
		std::cout << "This is result number: " << d << std::endl;
	} catch (const std::exception&) {
		return false;
	}
	pointer += 2;
	return true;
}
