#include<WinSock2.h>
#include<ws2tcpip.h>
#include<iostream>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "9090"
#define DEFAULT_BUFLEN 512

int trim(string &s)
{
	if (s.empty())
	{
		return 0;
	}

	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return 0;
}
int main()
{
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	// WSAData结构体包含了windows sockets的实现，
	WSAData wsaData;
	int ret;
	// 第一个参数设置版本，第二个参数将windows sockets实现进行初始化。
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		cout << "WSAStartup failed:" << ret << endl;
		return -1;
	}
	cout << "WSAStartup is success!" << " ret is : " << ret << endl;
	// 使用0填充hints内存区域
	ZeroMemory(&hints, sizeof(hints));
	// 指定协议族为IPv4
	hints.ai_family = AF_INET;
	// 指定socket类型为SOCK_STREAM(流式套接字)
	hints.ai_socktype = SOCK_STREAM;
	// 指定tcp协议
	hints.ai_protocol = IPPROTO_TCP;
	// AI_PASSIVE标志指示调用方打算在对绑定函数的调用中使用返回的套接字地址结构
	hints.ai_flags = AI_PASSIVE;
	// 将地址信息包装到result中
	ret = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (ret != 0)
	{
		cout << "getaddrinfo failed: " << ret << endl;
		WSACleanup();
		return -1;
	}
	cout << "getaddrinfo is success! " << "ret is : " << ret << endl;
	// 创建socket
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Error at socket()" << endl;
		return -1;
	}
	cout << "creat socket success!" << endl;
	// 将socket与address绑定
	ret = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR)
	{
		cout << "bind faild with error" << endl;
		return -1;
	}
	cout << "bind is success!" << endl;
	freeaddrinfo(result);
	if (listen(listenSocket, SOMAXCONN)== SOCKET_ERROR)
	{
		cout << "listen failed!" << endl;
		return -1;
	}
	cout << "server is running" << endl;
	// 创建socket用来保存接收到的连接
	SOCKET clientSocket;
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "clientSocket is vnalid!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}
	do // 使用do-while从套接字里不断读取数据，直到读取完成。
	{
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			cout << "Bytes received: " << iResult << endl;
			cout << "data is:" << trim(recvbuf) << endl;
			iSendResult = send(clientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "send failed" << endl;
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}
			cout << "Bytes sent: " << iSendResult << endl;
		}
		else if (iResult == 0)
		{
			cout << "Connection closing..." << endl;
		}
		else
		{
			cout << "recv failed" << endl;
			closesocket(clientSocket);
			WSACleanup();
			return -1;
		}
	} while (iResult > 0);
	ret = shutdown(clientSocket, SD_SEND);
	if (ret == SOCKET_ERROR)
	{
		cout << "shutdown failed" << endl;
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	}
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}