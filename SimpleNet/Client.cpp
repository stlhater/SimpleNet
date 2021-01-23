#include "main.h"
#include "Client.h"

void ReceiveThreadProc(Client* self)
{
	std::cout << "Client thread created";
	while (self->m_Socket.IsOpened()) {
		Packet* p = self->m_Socket.Recieve();
		if (p) {
			if (self->m_DiffieHellmanData.isSecurityInitialized)
				TransformBs(&p->data, self->m_DiffieHellmanData.end_key);
			self->m_Queue.push_back(Packet(*p));
			delete p;
		}
	}
}

#ifdef TEST
bool Client::Connect(const char* ip, unsigned short port, BigInt p, BigInt g, BigInt private_num)
{
	m_Socket.CreateSocket(port, ip, 10);
	if (m_Socket.Connect(ip, port)) {
		m_ulServerIP = m_Socket.GetIP();
		m_usServerPort = m_Socket.GetPort();

		//Initial
		m_DiffieHellmanData.p = p.to_int();
		m_DiffieHellmanData.g = g;
		m_DiffieHellmanData.private_num = private_num.to_int();

		m_DiffieHellmanData.public_cli = pow(m_DiffieHellmanData.g, m_DiffieHellmanData.private_num) % m_DiffieHellmanData.p;
		m_DiffieHellmanData.isSecurityInitialized = false;
		ByteStream bs;
		bs.Write((byte)ePacket::ID_CONNECT);
		bs.Write(m_DiffieHellmanData.p);
		bs.Write((int)m_DiffieHellmanData.g.to_string().length());
		bs.Write(m_DiffieHellmanData.g);
		bs.Write((int)m_DiffieHellmanData.public_cli.to_string().length());
		bs.Write(m_DiffieHellmanData.public_cli);
		Send(bs);
		std::cout << "Diffie data:" << std::endl
			<< "p: " << m_DiffieHellmanData.p << std::endl
			<< "g: " << m_DiffieHellmanData.g << std::endl
			<< "private key: " << m_DiffieHellmanData.private_num << std::endl
			<< "public cli: " << m_DiffieHellmanData.public_cli << std::endl;

		m_ReceiveThread = thread(ReceiveThreadProc, this);
		m_ReceiveThread.detach();

		//Diffie
		while (!m_DiffieHellmanData.isSecurityInitialized) {
			Packet* p;
			int tries = 40;
			while ((p = Receive()) == nullptr) {
				tries--;
				if (tries < 0) {
					std::cout << "timeout";
					return false;
				}
				Sleep(10);
			}
			switch ((ePacket)p->id) {
			case ID_ACCEPT_CONNECT:
			{
				int digits;
				p->data.Read(digits);
				byte* data = (byte*)malloc(digits + 1);
				p->data.Read(data, digits);
				data[digits] = '\0';

				m_DiffieHellmanData.public_srv = BigInt(std::string((char*)data));
				m_DiffieHellmanData.end_key = pow(m_DiffieHellmanData.public_srv, m_DiffieHellmanData.private_num) % m_DiffieHellmanData.p;
				m_DiffieHellmanData.isSecurityInitialized = true;

				std::cout << "\nEND KEY:" << m_DiffieHellmanData.end_key << std::endl;
				std::cout << "\npublic srv:" << m_DiffieHellmanData.public_srv << std::endl;
				ByteStream bas;
				bas.Write((byte)ID_SUCCESFUL_CONNECTED);
				Send(bas);
				break;
			}
			case ID_DENY_CONNECT: // what a shame(((
				delete p;
				return false;
			}
			delete p; //deallocate...
		}
		//

		return true;
	}
}
#else
bool Client::Connect(const char* ip, unsigned short port)
{
	m_Socket.CreateSocket(port, ip, 10);
	if (m_Socket.Connect(ip, port)) {
		m_ulServerIP = m_Socket.GetIP();
		m_usServerPort = m_Socket.GetPort();

		//Initial
		m_DiffieHellmanData.p = gen_prime(rand() % 10 + 450, rand() % 30 + 550);
		m_DiffieHellmanData.g = gen_prime(3, rand() % 30 + 150);
		m_DiffieHellmanData.private_num = rand() % 48 + 3;
		
		m_DiffieHellmanData.public_cli = pow(m_DiffieHellmanData.g, m_DiffieHellmanData.private_num) % m_DiffieHellmanData.p;
		m_DiffieHellmanData.isSecurityInitialized = false;
		ByteStream bs;
		bs.Write((byte)ePacket::ID_CONNECT);
		bs.Write(m_DiffieHellmanData.p);
		bs.Write((int)m_DiffieHellmanData.g.to_string().length());
		bs.Write(m_DiffieHellmanData.g);
		bs.Write((int)m_DiffieHellmanData.public_cli.to_string().length());
		bs.Write(m_DiffieHellmanData.public_cli);
		Send(bs);
		std::cout << "Diffie data:" << std::endl
			<< "p: " << m_DiffieHellmanData.p << std::endl
			<< "g: " << m_DiffieHellmanData.g << std::endl
			<< "private key: " << m_DiffieHellmanData.private_num << std::endl
			<< "public cli: " << m_DiffieHellmanData.public_cli << std::endl;

		m_ReceiveThread = thread(ReceiveThreadProc, this);
		m_ReceiveThread.detach();

		//Diffie
		while (!m_DiffieHellmanData.isSecurityInitialized) {	
			Packet* p;
			int tries = 40;
			while ((p = Receive()) == nullptr) {
				tries--;
				if (tries < 0) {
					std::cout << "timeout";
					return false;
				}
				Sleep(10);
			}
			switch ((ePacket)p->id) {
			case ID_ACCEPT_CONNECT:
			{
				int digits;
				p->data.Read(digits);
				byte* data = (byte*)malloc(digits + 1);
				p->data.Read(data, digits);
				data[digits] = '\0';

				m_DiffieHellmanData.public_srv = BigInt(std::string((char*)data));
				m_DiffieHellmanData.end_key = pow(m_DiffieHellmanData.public_srv, m_DiffieHellmanData.private_num) % m_DiffieHellmanData.p;
				m_DiffieHellmanData.isSecurityInitialized = true;

				std::cout << "\nEND KEY:" << m_DiffieHellmanData.end_key << std::endl;
				std::cout << "\npublic srv:" << m_DiffieHellmanData.public_srv << std::endl;
				ByteStream bas;
				bas.Write((byte)ID_SUCCESFUL_CONNECTED);
				Send(bas);
				break;
			}
			case ID_DENY_CONNECT: // what a shame(((
				delete p;
				return false;
			}
			delete p; //deallocate...
		}
		//

		return true;
	}
}
#endif
void Client::Disconnect(int reason)
{
	ByteStream bs;
	bs.Write((byte)ePacket::ID_DISCONNECT);
	bs.Write(reason);
	Send(bs);
}

void Client::Send(ByteStream& bs)
{
	m_Socket.Send(bs, m_ulServerIP, m_usServerPort, true);
}

Packet* Client::Receive()
{
	if (m_Queue.size() > 0) {
		Packet* pack = new Packet(m_Queue.front());
		m_Queue.pop_front();
		return pack;
	}
	return nullptr;
}

Client::~Client()
{
	Disconnect(228);
}