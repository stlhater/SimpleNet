#pragma once
#include "main.h"

#ifdef WIN32 || WIN64
#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <winsock.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

typedef int socklen_t;
#endif
//Linux? no.

class SocketBase
{
public:
	SocketBase();
	~SocketBase();
	//if ip is null - socket will be listen
	void CreateSocket(unsigned short port, const char* ip, unsigned int timeout);
	void CloseSocket();
	bool Connect(const char* ip, unsigned short port);
	Packet* Recieve();
	bool IsOpened();
	void Send(ByteStream& bs, unsigned long addr, unsigned short port, bool auto_htons = true);
	unsigned long GetIP();
	unsigned short GetPort();
private:
	unsigned long m_ulIp;
	unsigned short m_usPort;
	SOCKET m_Socket;
	bool m_bIsSocketOpened;
};

