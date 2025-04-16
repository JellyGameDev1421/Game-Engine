#include <DemoEngine_PCH.h>
#include "Server.h"

namespace DemoEngine
{
	struct SimpleMessage {
		int packetType = 99;
		char message[256];
	};

	ServerClass::ServerClass()
	{
		if (enet_initialize() != 0)
			std::cout << "Enet failed to initialise!\n";

		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = 9050;

		server = enet_host_create(&address, 32, 2, 0, 0);
		if (server == nullptr)
			std::cout << "Server failed to initialise!\n";
	}

	ServerClass::~ServerClass()
	{
		enet_host_destroy(server);
		atexit(enet_deinitialize);
	}

	void ServerClass::ConnectToClient()
	{
		if (!server)
			return;

		while (enet_host_service(server, &enetEvent, 0) > 0)
		{
			LOG_INFO("Waiting for packets...");

			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				LOG_INFO("A client connected!");
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(enetEvent.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				LOG_WARN("Client disconnected.");
				break;
			}
		}
	}
}
