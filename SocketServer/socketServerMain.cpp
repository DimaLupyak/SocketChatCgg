#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <list>
#include <string>
#include <conio.h>
//using namespace std;

class SocketServer
{
	private:
		WSADATA WsaData;
		SOCKET Socket;
		SOCKADDR_IN server;
		HANDLE Thread1;
		void Receive();
		std::list<SOCKET> clients;
	public:
		SocketServer(int port);
		void Start(int maxClientCount);
		
};

SocketServer::SocketServer(int port = 7898)
{
	if (WSAStartup(MAKEWORD(2,2), &WsaData) != 0)
	{    	
    	throw("WSA FAILED\n");
    }
 
    Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket == SOCKET_ERROR)
	{
		throw("Socket Failed to load\n");    	
    }
 
    server.sin_port = htons(port);
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(Socket, (SOCKADDR *)(&server), sizeof (server)) == SOCKET_ERROR) 
    { 
    	throw("BINDING FAILED\n");
    }
}
void SocketServer::Receive()
{
	do {
    	if(listen(Socket, 5000) == 0)
		{
			SOCKADDR_IN from;
			int fromlen=sizeof(from);
			SOCKET client = accept(Socket,(struct sockaddr*)&from, &fromlen);
			if(client == SOCKET_ERROR)
			{
				std::cout<<"ACCEPT FAILED\n";
			}
			else
			{
				std::cout<<"Client connected\n";
				clients.push_back(client);
				char recvBuffer[256];
				while(1)
				{
					int recvResult = recv(client,recvBuffer,sizeof(recvBuffer),0);
					if(recvResult == SOCKET_ERROR)break;
					for( int i = 0; i < recvResult; i++ )
					{
						std::cout<<recvBuffer[i];
					}
					std::cout<<std::endl;
					for (std::list<SOCKET>::iterator i=clients.begin(); i != clients.end(); ++i)
					{
						if(*i!=client)
						{
							
						send(*i,recvBuffer,recvResult,MSG_DONTROUTE);
						}
					}         			
				}
				std::cout<<"Client disconnected\n";
				clients.remove(client);
			}
		}
    } while (Socket != SOCKET_ERROR);
}

void SocketServer::Start(int maxClientCount)
{
	std::thread *threads = new std::thread[maxClientCount];
	for(int i = 0; i<maxClientCount; i++)
	{
		threads[i]=std::thread(&SocketServer::Receive, this);
	}
}
#define MAX_CLIENT_COUNT 10
int main(int argc, char *argv[]) 
{
	SocketServer* socketServer = new SocketServer();
	socketServer->Start(MAX_CLIENT_COUNT);
	while (getch()!=27)	{}
}

