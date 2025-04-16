#include <DemoEngine_PCH.h>
#include "Client.h"

namespace DemoEngine
{
	struct SimpleMessage {
		int packetType = 99;
		char message[256];
	};

	void ClientClass::ConnectClient()
	{
		if (enet_initialize() != 0)
			std::cout << "ENet failed to initialise!\n";

		client = enet_host_create(NULL, 1, 2, 0, 0);

		if (!client)
		{
			std::cout << "Client failed to initialise!\n";
			return;
		}

		enet_address_set_host(&address, "127.0.0.1");
		address.port = 9050;

		peer = enet_host_connect(client, &address, 2, 0);
		if (!peer)
		{
			std::cout << "No available peers.\n";
			return;
		}

		// Create a simple handshake message
		SimpleMessage msg;
		msg.packetType = 99;
		strcpy_s(msg.message, "Hello from client!");

		ENetPacket* packet = enet_packet_create(&msg, sizeof(SimpleMessage), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}

	void ClientClass::ReceiveData()
	{
		if (!client)
			return;

		while (enet_host_service(client, &enetEvent, 0) > 0)
		{
			LOG_INFO("Waiting for packets...");

			if (enetEvent.type == ENET_EVENT_TYPE_RECEIVE)
			{
				enet_packet_destroy(enetEvent.packet);
			}
		}
	}

	void ClientClass::Cleanup()
	{
		enet_host_destroy(client);
		atexit(enet_deinitialize);
	}
}
