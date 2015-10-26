#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <iostream>
#include <thread>
using namespace std;

class SocketClient
{
	private:
		WSADATA WsaData;
		SOCKET Socket;
		SOCKADDR_IN anAddr;
		void Receive();
		void Send();
		char* clientName;
	public:
		SocketClient(char* clientName, char* addres, int port);
		void Start();
		
};

SocketClient::SocketClient(char* clientName = "anonym", char* addres = "127.0.0.1", int port = 7898)
{
	this->clientName = clientName;
	if (WSAStartup(MAKEWORD(2,2), &WsaData) != 0)
	{    	
    	throw("WSA FAILED\n");
    }
 
    Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket == SOCKET_ERROR)
	{
		throw("Socket Failed to load\n");    	
    }
 
    anAddr.sin_port = htons(port);
    anAddr.sin_family = AF_INET; 
    anAddr.sin_addr.s_addr = inet_addr(addres);
    if (connect(Socket, (struct sockaddr *)&anAddr, sizeof (anAddr)) == SOCKET_ERROR) 
    { 
    	throw("CONNECTING FAILED\n");
    }
}
void SocketClient::Start()
{
	std::thread receive(&SocketClient::Receive, this);
	std::thread send(&SocketClient::Send, this);
	receive.join();
	send.join();
}	
void SocketClient::Receive()
{
	char recvBuffer[256];
	while(1)
	{
		int recvResult = recv(Socket,recvBuffer,sizeof(recvBuffer),0);
		if(recvResult == SOCKET_ERROR)break;
		for( int i = 0; i < recvResult; i++ )
		{
			std::cout<<recvBuffer[i];
		}
		std::cout<<std::endl;
	
	}
}
void SocketClient::Send()
{
	while(1)
	{
		string input;
		std::getline(std::cin,input);
		string message = clientName;		
		message += ": ";		
		message += input;
		const char* SendBuffer = message.c_str();
		send(Socket,SendBuffer,sizeof(char)*message.size(),MSG_DONTROUTE);			
	}
}

int main(int argc, char *argv[]) 
{
	SocketClient* socketClient;
	cout<<"Please enter your name: ";
	string input;
	
	cin>>input;
	char *clientName = new char[input.length() + 1];
	strcpy(clientName, input.c_str());
	bool allGood;
	do
	{
		allGood = true;
		cout<<"Please enter server adress: ";
		input;
		
		cin>>input;
		char *serverAdress = new char[input.length() + 1];
		strcpy(serverAdress, input.c_str());
		try
		{
			socketClient = new SocketClient(clientName, serverAdress);
		}
		catch(...)
		{
			allGood = false;
		}	
	}
	while(allGood==false);	
	
	socketClient->Start();
	while (getch()!=27)	{}
	delete [] clientName;	
}

