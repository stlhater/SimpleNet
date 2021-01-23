#pragma once
#include "main.h"
#include <thread>
#include <list>
#include "SocketBase.h"


using std::thread;
using std::list;

struct DiffieHellmanData {
	int p;
	BigInt g;
	int private_num;
	BigInt public_srv;
	BigInt public_cli;
	BigInt end_key;
	bool isSecurityInitialized;
};

class Client
{
public:
#ifdef TEST
	bool Connect(const char* ip, unsigned short port, BigInt p, BigInt g, BigInt private_num);
#else 
	bool Connect(const char* ip, unsigned short port);
#endif
	void Disconnect(int reason);
	void Send(ByteStream& bs);
	Packet* Receive();
	friend void ReceiveThreadProc(Client *self);
	~Client();
private:
	SocketBase m_Socket;
	DiffieHellmanData m_DiffieHellmanData;
	unsigned long m_ulServerIP;
	unsigned short m_usServerPort;
	list<Packet> m_Queue;
	thread m_ReceiveThread;
};

