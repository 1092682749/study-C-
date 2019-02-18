#include<WinSock2.h>
#include<iostream>
#include<ws2tcpip.h>
#include<sstream>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define FUN fun1(fun2)
struct SocketData
{
	SOCKET sock;
};
void fun1(void (*name)(const char* str))
{
	cout << "fun1" << endl;
	name("fun2");
}
void fun2(const char* str)
{
	cout << str << endl;
}
DWORD WINAPI analysisRequest(LPVOID lpParam)
{
	struct SocketData *sd = (SocketData*)lpParam;
	SOCKET sock = sd->sock;
	int ret = 0;
	char recvbuf[10240];
	//memset(recvbuf, 0, 10240);
	int recvlen = 10240;
	ostringstream os;
	do
	{
		ret = recv(sock, recvbuf, recvlen, 0);
		recvbuf[ret] = 0;
		send(sock, recvbuf, ret, 0);
		if (ret > 0)
		{
			os << recvbuf;
		}
	} while (ret > 0);
	cout << "recvbuf size is:" << sizeof(recvbuf) << endl;
	cout << os.str();
	return 0;
}

// 3
int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//WSADuplicateSocketA();
	//FUN;
	struct addrinfo *result, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	getaddrinfo(NULL, "9090", &hints, &result);

	SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(sock, result->ai_addr, result->ai_addrlen);
	listen(sock, 10);
	while (true)
	{
		DWORD tid;
		SOCKET clientSocket = INVALID_SOCKET;
		clientSocket = accept(sock, NULL, NULL);
		// 创建socketdata数据
		struct SocketData *sd = (SocketData*)malloc(sizeof(SocketData));
		sd->sock = clientSocket;
		if (clientSocket != INVALID_SOCKET)
		{
			HANDLE t = CreateThread(NULL, 0, analysisRequest, sd, 0, &tid);
		}
		/*thread t(analysisRequest, clientSocket);
		t.detach();*/
		/*int ret = 0;
		char recvbuf[10240];
		int recvlen = 10240;
		ostringstream os;
		do
		{
			ret = recv(clientSocket, recvbuf, recvlen, 0);
			send(clientSocket, recvbuf, ret, 0);
			os << recvbuf;
		} while (ret > 0);
		cout << os.str();
		shutdown(clientSocket, SD_SEND);
		closesocket(clientSocket);*/
	}
	return 0;
}