/**
 * @file   Utils.cpp
 * @brief  Defines the Utils class.
 * @author Harshal Dhagale
 */

#include "Utils.hpp"

const std::string utl::Utils::local_addr = "http://localhost/";

utl::Utils::Utils() {}

struct utl::URL* utl::Utils::parse_url(std::string url) {
	utl::URL* parsed_url = new utl::URL;
	std::string request_method;
	std::string requestor;
	std::string curr;
	int taken = 0;
	// Loop index i is int; string::size() returns size_t. static_cast keeps comparisons and
	// indexing explicit so we don't mix signed/unsigned by accident (avoids warnings and bugs
	// if values were ever large enough to matter). operator[] takes size_t, so i is cast when indexing.
	for (int i =0; i < static_cast<int>(url.size()); i++) {
		if (url[static_cast<size_t>(i)] == ' ') {
			if (taken==1) {
				parsed_url->requestor = curr;
				curr.clear();
                taken++;
			}
            else if(!taken){
                parsed_url->request_method = curr;
                curr.clear();
                taken++;
            }
			continue;
		}
		curr += url[static_cast<size_t>(i)];
	}
    parsed_url->version = curr;
	
	return parsed_url;
}
