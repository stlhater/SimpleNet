#include "main.h"
#include "Server.h"

void ReceiveThreadProc(Server* self)
{
	std::cout << "Server thread created";
	while (self->m_Socket.IsOpened()) {
		Packet* p = self->m_Socket.Recieve();
		if (p) {
			if (self->ExistClient(p->sender, p->sender_port))
				if (self->FindClient(p->sender, p->sender_port).isSecurityInitialized)
					TransformBs(&p->data, self->FindClient(p->sender, p->sender_port).end_key);
			self->m_Queue.push_back(Packet(*p));
			delete p;
		}
	}
}

bool Server::Start(int port)
{
	m_Socket.CreateSocket(port, NULL, 10);
	m_ReceiveThread = thread(ReceiveThreadProc, this);
	m_ReceiveThread.detach();
}

Packet* Server::Receive()
{
	if (m_Queue.size() > 0) {
		Packet* pack = new Packet(m_Queue.front());

		if (pack->id == ID_CONNECT) {
			if (!ExistClient(pack->sender, pack->sender_port)) {
				m_ClientList.push_back(ClientInfo(pack->sender, pack->sender_port));
				stConnect connect;
				pack->data.Read(connect.p);

				pack->data.Read(connect.len_g);
				connect.g = (byte*)malloc(connect.len_g + 1);
				pack->data.Read(connect.g, connect.len_g);
				connect.g[connect.len_g] = '\0';

				pack->data.Read(connect.len_public_cli);
				connect.public_cli = (byte*)malloc(connect.len_public_cli);
				pack->data.Read(connect.public_cli, connect.len_public_cli);
				connect.public_cli[connect.len_public_cli] = '\0';

				GetClient(pack->sender, pack->sender_port).p = connect.p;
				GetClient(pack->sender, pack->sender_port).g = BigInt(std::string((char*)connect.g));
				GetClient(pack->sender, pack->sender_port).public_cli = BigInt(std::string((char*)connect.public_cli));
				GetClient(pack->sender, pack->sender_port).private_num = rand() % 46 + 3;
				
				GetClient(pack->sender, pack->sender_port).end_key = pow(GetClient(pack->sender, pack->sender_port).public_cli, GetClient(pack->sender, pack->sender_port).private_num) % connect.p;
				GetClient(pack->sender, pack->sender_port).isSecurityInitialized = true;

				
				BigInt public_key = pow(GetClient(pack->sender, pack->sender_port).g, GetClient(pack->sender, pack->sender_port).private_num) % connect.p;
				ByteStream bs;
				bs.Write((byte)ePacket::ID_ACCEPT_CONNECT);
				bs.Write((int)public_key.to_string().length());
				bs.Write(public_key);
				Send(pack->sender, pack->sender_port, bs);
				std::cout << "\nEND KEY:" << GetClient(pack->sender, pack->sender_port).end_key << std::endl << std::endl;
				std::cout << "Diffie data:" << std::endl
					<< "p: " << connect.p << std::endl
					<< "g: " << connect.g << std::endl
					<< "private key: " << GetClient(pack->sender, pack->sender_port).private_num << std::endl
					<< "public cli: " << connect.public_cli << std::endl
					<< "public srv: " << public_key << std::endl
					<< "end key: " << GetClient(pack->sender, pack->sender_port).end_key << std::endl;
			}
			else {
				ByteStream bs;
				bs.Write((byte)ePacket::ID_DENY_CONNECT);
				Send(pack->sender, pack->sender_port, bs);
			}
			m_Queue.pop_front();
			delete pack;
			return nullptr;
		}
		if (pack->id == ID_DISCONNECT) {
			DeleteClient(pack->sender, pack->sender_port);
		}
		m_Queue.pop_front();
		return pack;
	}
	return nullptr;
}

void Server::Send(unsigned long ip, unsigned short port, ByteStream& bs)
{
	m_Socket.Send(bs, ip, port, false);
}

Server::~Server()
{
	m_Socket.CloseSocket();
}

ClientInfo Server::FindClient(unsigned long ip, unsigned short port)
{
	std::for_each(m_ClientList.begin(), m_ClientList.end(), [&](ClientInfo c) {
		if (c.ip == ip && c.port == port) return c;
	});
	return ClientInfo(0,0);
}

bool Server::ExistClient(unsigned long ip, unsigned short port)
{
	std::for_each(m_ClientList.begin(), m_ClientList.end(), [&](ClientInfo c) {
		if (c.ip == ip && c.port == port) return true;
	});
	return false;
}

ClientInfo& Server::GetClient(unsigned long ip, unsigned short port)
{
	/*
	std::for_each(m_ClientList.begin(), m_ClientList.end(), [&](ClientInfo &c) {
		if (c.ip == ip && c.port == port) return c;
	});
	*/
	for (auto it = m_ClientList.begin(); it != m_ClientList.end(); it++) {
		if ((*it).ip == ip && (*it).port == port) {
			return *it;
		}
	}
	return m_ClientList.front();
}

void Server::DeleteClient(unsigned long ip, unsigned short port)
{
	for (auto it = m_ClientList.begin(); it != m_ClientList.end(); it++) {
		if ((*it).ip == ip && (*it).port == port) {
			m_ClientList.erase(it);
			break;
		}
	}
}


