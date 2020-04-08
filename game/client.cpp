#include <iostream>
#include "client.h"

unsigned rsize = 70;
unsigned csize = 71;

// g++ -std=c++11 -pthread client.cpp -o c
int main(int argc, char const *argv[]){
		
	std::string ip = "127.0.0.1";
	int port = 8888;

	Client *cl = new Client();
	cl->Connect(ip, port);
	cl->Talking();

	delete cl;
	return 0;
}