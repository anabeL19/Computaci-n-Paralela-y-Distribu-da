#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

extern unsigned rsize;
extern unsigned csize;

void print_table_from_str(std::string _t, unsigned _r, unsigned _c){
	for(unsigned i=0; i<_r; i++){
		for(unsigned j=0; j<_c; j++)
			std::cout << _t[(i*_c)+j];
		std::cout << "\n";
	}
}

void print_road(std::string _road, unsigned r, unsigned c){
	unsigned i,j;
	std::string tmp = "";
	for(j=0; j<(c-3)/2; j++)
		tmp += " ";

	for(i=0; i<r; i++){
		std::cout << "|" << tmp << _road[i] << tmp << "|\n";
	}
}

void thread_write(int _sockFD){
	std::string text;

	while(true){
		getline(std::cin, text);
		if(text == "end"){
			break;
		}
		send(_sockFD, text.c_str(), text.size(), 0);
	}
}

void thread_read(int _sockFD, unsigned _rsize, unsigned _csize){
	unsigned buffer_size = _rsize*_csize;
	char buffer[buffer_size];

	int n;

	while(true){
		memset(&buffer, 0, buffer_size);
		n = recv(_sockFD, buffer, buffer_size, 0);
		if(n == 0){
			system("clear");
			std::cout << "you are lose...\n";
			break;
		}
		else if(n < 0){
			perror("error: receiving text");
		}
		else{
			system("clear");
			if (buffer[0]=='0'){
				std::string text;
				std::cout<<"enter Avatar again"<<std::endl;
				getline(std::cin, text);
				send(_sockFD, text.c_str(), text.size(), 0);
			}
			print_table_from_str(std::string(buffer),_rsize,_csize);
		}
	}


}

class Client{
private:
    int m_client_sock;
    struct sockaddr_in m_client_addr;
public:
    Client();
    void Connect(std::string, int);
    void Talking();
};

Client::Client(){
    this->m_client_sock = socket(AF_INET, SOCK_STREAM, 0);
}

void Client::Connect(std::string _ip, int _port){
    if(m_client_sock < 0){
        perror("can't create socket!");
        exit(0);
    }

    memset(&m_client_addr, 0, sizeof(m_client_addr));
    m_client_addr.sin_family = AF_INET;
    m_client_addr.sin_port = htons(_port);
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_client_addr.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_client_sock);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_client_sock);
      exit(EXIT_FAILURE);
    }

    if (connect(m_client_sock, (const struct sockaddr *)&m_client_addr, sizeof(m_client_addr)) < 0){
        perror("connect failed"); 
        exit(1);
    }

    std::cout << "Client connected\n";
}

void Client::Talking(){
    std::cout << "client talking\n";

    std::thread tread(thread_read,   m_client_sock, rsize, csize);
    std::thread twrite(thread_write, m_client_sock);

    tread.join();
    twrite.join();
    close(m_client_sock);
}

#endif