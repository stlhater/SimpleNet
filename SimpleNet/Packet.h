#pragma once
#include "main.h"

class Packet
{
public:
	byte id;
	ByteStream data;
	unsigned long sender;
	unsigned short sender_port;
	Packet(ByteStream& bs, unsigned long ip, unsigned short port) {
		id = 0;
		bs.Read(id);
		unsigned int data_len = bs.GetLength() - sizeof(byte);
		if (data_len > 0 && data_len != -1) {
			byte* pdata = (byte*)malloc(data_len);
			bs.Read(pdata, data_len);
			data.Write(pdata, data_len);
			free(pdata);
		}
		bs.ResetReadPointer();
		sender = ip;
		sender_port = port;
	}
};

