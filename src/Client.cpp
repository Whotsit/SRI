//Client.cpp
#include<fstream>
#include<iostream>

#include "Client.h"
using namespace std;

Client::Client(){
	client = new TCPSocket("0.0.0.0",9999,100);
}

void Client::UploadFile(string fileName){
	if(fileName.size() >= 5){
		if(fileName.substr(fileName.size()-4,4) != ".sri"){
			cout << "Please enter a valid .sri file\n";
			return;
		}
	}else{
		cout << "Please enter a valid .sri file\n";
		return;
	}

	string input;
	fstream file;
	file.open(fileName.c_str(),std::fstream::in);
	cout<<"Inputting From File: "<< fileName << endl;
	
	while(!file.eof()){
	  getline(file,input);
	  if(input.size() == 0) break;
	  //ParseLine(input); Send to the server
	}
	file.close();
}

void Client::ListenForInput(){
	char* remote = client.getRemoteAddress();
	TCPSocket* daServer = new TCPSocket(remote,9999,100);
	int maxBytes = 1024;
	char buffer[maxBytes];
	buffer[0] = 'a';
	int bytes_read = daServer->writeToSocket(buffer, maxBytes);
	cout << buffer;
	
	int bytes_read =  server.readFromSocket(buffer, maxBytes);
	cout << buffer;
	//while(true)
		//listen for string
		//send to server
		//Have server send quit command or have Client parse out 'q' and send server quit command?
		//Upload files locally and send over string by string or require files already on server side and just have it upload there?
		//  (this is less flexible but easier maybe?) Basically not doing this requires a mini-Parse on the client side. Which we could do.
}

int main(){
	Client* cliet = new Client();
	cliet->ListenForInput();
}