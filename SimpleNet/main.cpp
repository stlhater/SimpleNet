#include "main.h"
#include <istream>
#include <ostream>

#ifdef CLIENT
#include "Client.h"
#endif

#ifdef SERVER
#include "Server.h"
#endif


#ifdef SERVER
int main() {
	Server server;
	server.Start(7777);
	while (GetAsyncKeyState(VK_END) == 0) {
		Packet* p = server.Receive();
		if (p) {
			switch ((ePacket)p->id) {
			case ID_CONNECT:
				std::cout << "Connected new client. " << p->sender << " " << p->sender_port << std::endl;
				break;
			case ID_DISCONNECT:
				std::cout << "Disconnected client. " << p->sender << " " << p->sender_port << std::endl;
				break;
			}
			delete p;
		}
	}
	std::cout << "End.";
	system("pause");
	return 0;
}
#endif


#ifdef CLIENT
int main() {
	srand(GetTickCount());
	Client client;
	client.Connect("127.0.0.1", 7777);
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		Packet* p = client.Receive();
		if (p) {
			switch ((ePacket)p->id) {
			case ID_CONNECT:
				std::cout << "Connected new client. " << p->sender << " " << p->sender_port << std::endl;
				break;
			case ID_DISCONNECT:
				std::cout << "Disconnected client. " << p->sender << " " << p->sender_port << std::endl;
				break;
			}
			delete p;
		}
	}
	std::cout << "End.";
	system("pause");
	return 0;
}
#endif
