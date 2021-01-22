#include "main.h"
#include "SocketBase.h"

SocketBase::SocketBase() {
	m_usPort = 80;
	m_bIsSocketOpened = false;
#ifdef WIN32 || WIN64
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif
}

SocketBase::~SocketBase()
{
	if (m_bIsSocketOpened) {
		CloseSocket();
	}
#ifdef WIN32 || WIN64
	WSACleanup();
#endif
}

void SocketBase::CreateSocket(unsigned short port, const char* ip, unsigned int timeout)
{
	SOCKET ret = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	unsigned long opt = 1;
	if (setsockopt(ret, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(long)) != 0) {
		throw new std::exception("Cant set sock opt reuseaddr");
		ret = INVALID_SOCKET;
	}

	opt = timeout;
	if (setsockopt(ret, SOL_SOCKET, SO_SNDTIMEO, (char*)&opt, sizeof(long)) != 0) {
		throw new std::exception("Cant set sock opt sndtimeo");
		ret = INVALID_SOCKET;
	}
	if (setsockopt(ret, SOL_SOCKET, SO_RCVTIMEO, (char*)&opt, sizeof(long)) != 0) {
		throw new std::exception("Cant set sock opt rcvtimeo");
		ret = INVALID_SOCKET;
	}
	unsigned long nonblocking = 1;
	if (ioctlsocket(ret, FIONBIO, &nonblocking) != 0)
	{
		throw new std::exception("Cant unblock socket");
		ret = INVALID_SOCKET;
	}

	sockaddr_in srv, cli;

	srv.sin_family = AF_INET;
	srv.sin_port = htons(port);
	m_usPort = port;

	if (ip && ip[0]) {
		srv.sin_addr.s_addr = inet_addr(ip);
		m_ulIp = srv.sin_addr.s_addr;
		m_bIsSocketOpened = true;
	}
	else {
		srv.sin_addr.s_addr = INADDR_ANY;
		if (bind(ret, (sockaddr*)&srv, sizeof(sockaddr)) < 0) {
			throw new std::exception("Cant bind socket");
			ret = INVALID_SOCKET;
		}
		else {
			m_bIsSocketOpened = true;
		}
	}
	
	m_Socket = ret;
}

void SocketBase::CloseSocket()
{
	m_bIsSocketOpened = false;
	closesocket(m_Socket);
}

bool SocketBase::Connect(const char* ip, unsigned short port)
{
	sockaddr_in sock_addr;

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = inet_addr(ip);

	return !(connect(m_Socket, (struct sockaddr*) & sock_addr, sizeof(struct sockaddr)) != 0);
}

Packet* SocketBase::Recieve()
{
	int len;
	socklen_t size = sizeof(sockaddr_in);
	char buf[512];
	sockaddr_in cli;
	cli.sin_family = AF_INET;
	len = recvfrom(m_Socket, buf, 512, 0, (sockaddr*)&cli, &size);
	if (len != -1) {
		ByteStream bs;
		bs.Write((byte*)buf, len);
		return new Packet(bs, cli.sin_addr.S_un.S_addr, cli.sin_port);
	}
	return nullptr;
}

bool SocketBase::IsOpened()
{
	return m_bIsSocketOpened;
}

void SocketBase::Send(ByteStream& bs, unsigned long addr, unsigned short port, bool auto_htons)
{
	sockaddr_in sa;
	sa.sin_port = auto_htons ? htons(port) : port;
	sa.sin_addr.s_addr = addr;
	sa.sin_family = AF_INET;
	unsigned int len = 0;
	unsigned int res = 0;
	unsigned char* data = bs.GetData(&len);
	do
	{
		res = sendto(m_Socket, (char*)data, len, 0, (const sockaddr*)&sa, sizeof(struct sockaddr_in));
	} while (res == 0);
}

unsigned long SocketBase::GetIP()
{
	return m_ulIp;
}

unsigned short SocketBase::GetPort()
{
	return m_usPort;
}
