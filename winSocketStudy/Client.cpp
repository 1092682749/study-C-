#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "9090"
#define DEFAULT_BUFLEN 512


int main_(int argc, char **argv)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int recvbuflen = DEFAULT_BUFLEN;
	const char *sendbuf = "this is a test message";
	char recvbuf[DEFAULT_BUFLEN];
	int ret;
	WSAData wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		cout << "WSAStartup failed ret is: " << ret << endl;
		return -1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	ret = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (ret != 0)
	{
		cout << "getaddrinfo failed" << endl;
		WSACleanup();
		return -1;
	}
	SOCKET connectionSocket = INVALID_SOCKET;
	ptr = result;
	connectionSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (connectionSocket == INVALID_SOCKET)
	{
		cout << "create socket failed" << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}
	cout << "create socket success!" << endl;
	ret = connect(connectionSocket, ptr->ai_addr, ptr->ai_addrlen);
	if (ret == SOCKET_ERROR)
	{
		cout << "connection failed" << endl;
		WSACleanup();
		return -1;
	}
	cout << "connection success!" << endl;
	ret = send(connectionSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (ret != SOCKET_ERROR)
	{
		cout << "send failed" << endl;
		closesocket(connectionSocket);
		WSACleanup();
		return -1;
	}
	cout << "send bytes : " << ret << endl;
	ret = shutdown(connectionSocket, SD_SEND);
	if (ret == SOCKET_ERROR)
	{
		cout << "shutdown failed" << endl;
		closesocket(connectionSocket);
		WSACleanup();
		return -1;
	}
	do
	{
		ret = recv(connectionSocket, recvbuf, recvbuflen, 0);
		if (ret > 0)
		{
			cout << "Bytes received" << ret << endl;
		}
		else if (ret == 0)
		{
			cout << "connection closed" << endl;
		}
		else
		{
			cout << "recv failed" << endl;
		}
	} while (ret > 0);
	closesocket(connectionSocket);
	WSACleanup();
	return 0;
}