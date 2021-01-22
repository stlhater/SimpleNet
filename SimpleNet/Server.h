#pragma once
#include "main.h"
#include <list>
#include <vector>
#include <thread>
#include "SocketBase.h"

using std::thread;
using std::vector;
using std::list;

struct ClientInfo {
	unsigned long ip;
	unsigned short port;
	int p;
	BigInt g;
	int private_num;
	BigInt public_srv;
	BigInt public_cli;
	BigInt end_key;
	bool isSecurityInitialized;
	ClientInfo(unsigned long ip, unsigned short port) {
		this->ip = ip;
		this->port = port;
		isSecurityInitialized = false;
	}
};

class Server
{
public:
	bool Start(int port);
	Packet* Receive();
	void Send(unsigned long ip, unsigned short port, ByteStream& bs);
	~Server();
	friend void ReceiveThreadProc(Server *self);
	ClientInfo FindClient(unsigned long ip, unsigned short port);
	bool ExistClient(unsigned long ip, unsigned short port);
	ClientInfo& GetClient(unsigned long ip, unsigned short port);
	void DeleteClient(unsigned long ip, unsigned short port);
private:
	list<ClientInfo> m_ClientList;
	thread m_ReceiveThread;
	list<Packet> m_Queue;
	SocketBase m_Socket;
};

