/**
 * @file   MultiClientServer.hpp
 * @brief  Declares the MultiClientServer class (public interface).
 * @author Harshal Dhagale
 */

#ifndef MultiClientServer_hpp
#define MultiClientServer_hpp

#include <stdio.h>
#include <iostream>
#include <thread>
#include "TestServer.hpp"
#include "Utilities/Utils.hpp"
#include "ServerObject.hpp"
#include <fstream>

namespace HDE {

class MultiClientServer : public SimpleServer{
public:
	MultiClientServer();
	void launch() override;

private:
	std::thread main_thread;
	std::vector<std::thread> child_threads;
	// std::vector<>x

	bool accepter() override;
	bool handler_threaded(std::unique_ptr<serverObject> childServerObject);

	void handler() override;
	void responder() override;


	void responder(std::unique_ptr<serverObject>& childServerObject);
    void writer(std::unique_ptr<serverObject>& childServerObject,std::string message);
    // void reader();
	void read_first_line(std::unique_ptr<serverObject>& childServerObject);
	void verify(std::unique_ptr<serverObject>& childServerObject);
};

}  // namespace HDE

#endif
