/**
 * @file   ListeningSocket.hpp
 * @brief  Declares the ListeningSocket class (public interface).
 * @author Harshal Dhagale
 */

#ifndef ListeningSocket_hpp
#define ListeningSocket_hpp

#include <stdio.h>

#include "BindingSocket.hpp"

namespace HDE {

class ListeningSocket : public BindingSocket {
public:
	ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog);
	void start_listening();
private:
	int backlog;
	int listening;
};

}  // namespace HDE

#endif
