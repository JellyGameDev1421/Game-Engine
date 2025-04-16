#pragma once
#include <iostream>
#include <DemoEngine_PCH.h>

using namespace std;

namespace DemoEngine 
{
	class ServerClass 
	{
	public:

		ServerClass();
		~ServerClass();

		void setAddress(ENetAddress address);
		void ConnectToClient();

	private:
		struct Vector2 {
			float x;
			float y;
		};

		struct PhysicsData {
			int packetType = 1;
			Vector2 positions[2];
		};

		struct ClientData {
			int packetType = 0;
			int clientIndex;
		};

		struct ClientPacket {
			int clientIndex;
			Vector2 position;
		};

		int clientCount = 0;

		ENetHost* server;
		ENetEvent enetEvent;

		ENetPacket* dataPacket;
	};
}