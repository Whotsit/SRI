//Server.cpp
#include "Server.h"
#include <iostream>
using namespace std;

Server::Server(const char * _address, int _port, int _backlog){
	server = TCPServer(address,port,backlog);
}

Server::Daemon(){
	server.initializeSocket();
	for( ;; ){
		TCPSocket* tcpSocket = server.getConnection();
		if(tcpSocket == NULL) break;
		int maxBytes = 1024;
		char buffer[maxBytes];
		memset(buffer,0,maxBytes);
		int bytes_read =  server.readFromSocket(buffer, maxBytes);
		cout << buffer;
		
		if(buffer[0] == 'a') buffer[0] = 'b';
		int bytes_sent = server.writeToSocket(buffer, maxBytes);
		cout << buffer;
	  //check if Socket in map
	    //if not add new (ID,Socket) to map
	  //access Socket from map
	  //do thing and send string back if needed
	}
}

int main(){
	Server* serve = new Server("0.0.0.0",9999,100);
	serve->Daemon();
}

