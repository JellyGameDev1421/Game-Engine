#pragma once
#include <iostream>
#include <DemoEngine_PCH.h>

using namespace std;
namespace DemoEngine 
{
	class ClientClass 
	{
	public:

		void ConnectClient();
		void Cleanup();
		void ReceiveData();

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

		ENetAddress address;
		ENetHost* client;
		ENetPeer* peer;
		ENetEvent enetEvent;

	};
}