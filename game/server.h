#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <ctime>
#include <cmath>
#include "car.h"

#include <iostream>

#include <string>

#include <pthread.h>
#include <string>
#include <thread>
#include <mutex>

extern unsigned rsize;
extern unsigned csize;

std::string table_to_str(char **_t, unsigned _r, unsigned _c){
	std::string tmp = "";

	for(unsigned i=0; i<_r; i++){
		for(unsigned j=0; j<_c; j++){
			tmp += _t[i][j];
		}						
	}

	return tmp;
}

//-----------------------------------
class Socket{
public:
	std::string m_name;
	int m_id;
	int m_sock;

	Socket(){	};
	~Socket(){	};

	void SetName(std::string);
	void SetId(int);
};

void Socket::SetName(std::string _name){
	this->m_name = _name;
}

void Socket::SetId(int _id){
	this->m_id = _id;
}

//---------------------------------
class Thread{
public:
	std::thread m_tid;

	Thread(){};
	~Thread();

	void Create(void (*callback)(Socket *, Car *), Socket *, Car *);
	void Join();
};

void Thread::Create(void (*callback)(Socket *, Car *), Socket *dat, Car *car){
	this->m_tid = std::thread(callback, dat, car);
}

void Thread::Join(){
	m_tid.join();
}

//-------------------------------------------------
class Server{
private:
	static std::mutex m_cmutex;
	static std::vector<Socket> m_clients;
	static std::vector<Car> m_cars;
	static std::vector<std::pair<int,int> > m_obstacle;

	struct sockaddr_in m_server_addr;
	static int m_server_sock;
	static char **m_table;

	static void SendToAllClients(std::string);
	static int  FindClientIdx(Socket *);	
	static std::string GetLine(int);
	static bool IsAvatarUsed(char);
	static void UpdateTable(int);
	static void ClearTable(std::string);
	static void CreateObstacle();	
	static void PrintBody(int);
	static void PrintObstacle();
public:
	Server();
	~Server();

	void Connect(int);	
	void CreateTable();

	static void Listening();
	static void Send();
	static void HandleClient(Socket *, Car *);
	static bool Verify_colision(int idx);
};

std::vector<Socket>	Server::m_clients;
std::vector<Car>		Server::m_cars;
int 					Server::m_server_sock;
std::mutex 				Server::m_cmutex;
char** 					Server::m_table;
std::vector<std::pair<int,int> > Server::m_obstacle;

Server::Server(){
	m_server_sock = socket(AF_INET, SOCK_STREAM, 0);

}
void Server::CreateObstacle(){
   srand(time(0));
      m_obstacle.push_back(std::make_pair(3,10));
	   m_obstacle.push_back(std::make_pair(6,20));
	  m_obstacle.push_back(std::make_pair(10,20));
	  m_obstacle.push_back(std::make_pair(14,30));
	  m_obstacle.push_back(std::make_pair(17,20));
	  m_obstacle.push_back(std::make_pair(21,20));
	  m_obstacle.push_back(std::make_pair(25,40));
	  m_obstacle.push_back(std::make_pair(29,15));
	  m_obstacle.push_back(std::make_pair(33,40));
	  m_obstacle.push_back(std::make_pair(39,45));
}
void Server::CreateTable(){
	this->m_table = new char*[rsize];

	unsigned i, j;

	for(i=0; i<rsize; i++){
		this->m_table[i] = new char[csize];

		for(j=0; j<csize; j++){
			this->m_table[i][j] = ' ';
		}
	}
	CreateObstacle();
}

void Server::ClearTable(std::string _line){
	unsigned i, j;
    
	for(i=0; i<rsize; i++){
		for(j=0; j<csize; j++){
			m_table[i][j] = ' ';
		}
	}
	

	for(i=1; i<rsize-1; i++){
		for(j=1; j<csize-1; j++){
			m_table[i][j] = ' ';
		}
	}

	for(i=0; i<rsize; i++){
		m_table[i][0] = _line[i];
		m_table[i][csize-1] = _line[i];
	}
}

void Server::PrintBody(int _idx){
	m_table[m_cars[_idx].m_x][m_cars[_idx].m_y] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x+1][m_cars[_idx].m_y+1] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x+1][m_cars[_idx].m_y] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x+1][m_cars[_idx].m_y-1] = m_cars[_idx].m_avatar;
}

void Server::PrintObstacle(){

  for (int i=0;i<m_obstacle.size();i++){
	  m_table[m_obstacle[i].first][m_obstacle[i].second] = '*';
	  m_obstacle[i].first=(m_obstacle[i].first+2)%rsize;
  }

  //verificar colision 
  std::string text;
   for(int i=0;i<m_cars.size();i++){
	   if (Verify_colision(i)){
		   std::cout<<"There is COLITION  "<<m_cars[i].m_avatar <<"  YA NO ES PARTE DEL JUEGO"<<std::endl;
		   text = "E" + m_cars[i].m_avatar;
		   SendToAllClients(text);

		   close(m_clients[i].m_sock); 
		   Server::m_clients.erase(Server::m_clients.begin()+i);
		   Server::m_cars.erase(Server::m_cars.begin()+i);
	   }
   }
	
}
void Server::UpdateTable(int _t){
	unsigned i, j;

	ClearTable(GetLine(_t));

	for(i=0; i<m_cars.size(); i++){
			PrintObstacle();
			PrintBody(i);
	}
}

void Server::Connect(int _port){
	int reuse = 1;	
    memset(&m_server_addr, 0, sizeof(sockaddr_in));

    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = INADDR_ANY;
    m_server_addr.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_server_sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_server_sock, (struct sockaddr *) &m_server_addr, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_server_sock, 5);
    std::cout << "Server Created!\n";
    std::cout << "listening!\n";
}

std::string Server::GetLine(int init){
	std::string _tab = "";
	std::string _t;

	switch(init){
		case 0:
			_t = "| ";
			break;
		case 1:
			_t = " |";
			break;
		default:
			break;
	}

	for(unsigned i=0; i<rsize; i+=2){
		_tab += _t;
	}

	return _tab;
}

void Server::Listening(){
	Socket	*cli;
	Thread	*thr;
	Car	*car;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_clientAddr;

	while(true){
		cli = new Socket();
		thr = new Thread();
		car = new Car();

		// block
		cli->m_sock = accept(m_server_sock, (struct sockaddr *) &m_clientAddr, &cli_size);
		/// m_server_scok: descriptor del scoket habilitado para recibir conexiones.
	
	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else{	    	
	    	cli->SetName(inet_ntoa(m_clientAddr.sin_addr));
	        thr->Create(Server::HandleClient, cli, car);
	    }
	}
}
bool Server::Verify_colision(int idx){

	for(int i=0;i<m_obstacle.size();i++){
		if(m_obstacle[i].first==m_cars[idx].m_x && m_obstacle[i].second==m_cars[idx].m_y){
			return true;
		}
		if(m_obstacle[i].first==m_cars[idx].m_x+1 && m_obstacle[i].second==m_cars[idx].m_y+1){
			return true;
		}
		if(m_obstacle[i].first==m_cars[idx].m_x+1 && m_obstacle[i].second==m_cars[idx].m_y){
			return true;
		}
		if(m_obstacle[i].first==m_cars[idx].m_x+1 && m_obstacle[i].second==m_cars[idx].m_y-1){
			return true;
		}
	}
	return false;

}
void Server::HandleClient(Socket *cli, Car *car){

	char buffer[256];
	std::string text = "";

	int n;

	Server::m_cmutex.lock();

		cli->SetId(Server::m_clients.size());
	
		bool avatar = true;
		while(avatar){
			memset(buffer, 0, sizeof(buffer));
			n = recv(cli->m_sock, buffer, sizeof(buffer), 0); ///
			
			if(n > 0){
				if(!Server::IsAvatarUsed(buffer[0])){
					car->SetAvatar(buffer[0]);
					avatar = false;

					text = "1";
					send(cli->m_sock, text.c_str(), text.size(), 0);
				}
				else{
					text = "0"; ///Confirma que ese avatar ya esta
					send(cli->m_sock, text.c_str(), text.size(), 0);
				}
			}
		}

		srand(time(NULL));
		unsigned tr = rsize-2;
		unsigned tc = 2+(rand()%(csize-3));
        std::cout << "[" << tr << "," << tc << "]\n";
		car->SetPosition(tr, tc);
		std::cout << "client: " << cli->m_name << " connected\tid: " << cli->m_id << "\n";

		Server::m_clients.push_back(*cli);
		Server::m_cars.push_back(*car);

	Server::m_cmutex.unlock();
	
	int idx;
	bool t = true;
	while(t){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0); ///Retorna el # bytes usado,
		///recv ( int socketfd, void *buf, , int len, unsiged flag)

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			Server::m_cmutex.lock();
				idx = Server::FindClientIdx(cli);
				Server::m_clients.erase(Server::m_clients.begin()+idx);
				Server::m_cars.erase(Server::m_cars.begin()+idx);
			Server::m_cmutex.unlock();
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			idx = Server::FindClientIdx(cli);

			t = m_cars[idx].Move(buffer[0], rsize, csize);

			if (Verify_colision(idx)) {
				t=false;
			}

			if(!t){
				text = "";

				send(cli->m_sock, text.c_str(), text.size(), 0);
				std::cout << cli->m_name << " disconneted\n";
				close(cli->m_sock);

				Server::m_cmutex.lock();
					idx = Server::FindClientIdx(cli);
					Server::m_clients.erase(Server::m_clients.begin()+idx);
					Server::m_cars.erase(Server::m_cars.begin()+idx);
				Server::m_cmutex.unlock();
			}
		}
	}
}

void Server::Send(){
	int _sleep = 800;
	int initial = 0;

	std::string text;

	while(true){
		std::this_thread::sleep_for(std::chrono::milliseconds(_sleep));
		UpdateTable(initial%2);
		text = table_to_str(m_table, rsize, csize);
		SendToAllClients(text);
		initial++;
	}
}

void Server::SendToAllClients(std::string _text){
	Server::m_cmutex.lock();
		for(unsigned i=0; i<m_clients.size(); i++){			
			send(Server::m_clients[i].m_sock, _text.c_str(), _text.size(), 0);
		}
	Server::m_cmutex.unlock();
}

int Server::FindClientIdx(Socket *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(Server::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

bool Server::IsAvatarUsed(char _a){
	for(unsigned i=0; i<m_cars.size(); i++)
		if(m_cars[i].m_avatar == _a)
			return true;

	return false;
}

Server::~Server(){
	m_clients.clear();
}

#endif