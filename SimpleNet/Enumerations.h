#pragma once
#include "main.h"

enum ePacket {
	//Client
	ID_DISCONNECT = 10,
	ID_CONNECT,
	ID_SUCCESFUL_CONNECTED,
	//Server
	ID_ACCEPT_CONNECT = 40,
	ID_DENY_CONNECT
};

#pragma pack(0)
struct stDisconnect {
	int reason;
};

//Client -> Server
struct stConnect {
	int p;
	int len_g;
	byte* g;
	int len_public_cli;
	byte* public_cli;
};

//Server -> Client
struct stAcceptConnect {
	int len_public_srv;
	byte* public_srv;
};
