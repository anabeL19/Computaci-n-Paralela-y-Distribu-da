#include <iostream>
#include "server.h"

unsigned rsize = 70;
unsigned csize = 71;

// g++ -std=c++11 -pthread server.cpp -o s
int main(int argc, char const *argv[]){
	int port = 8888;		// port to clients
	Server *s = new Server();

	s->CreateTable();
	s->Connect(port);

	std::thread tlistening(Server::Listening);
	std::thread tsending(Server::Send);

	tlistening.join();

	delete s;
	return 0;
}