#define WIN32

#include "NetServer.h"

void NetServer::RegisterHandlers(bool (*hConnect)(ENetPeer* source),
	void (*hDisconnect)(int reason, ENetPeer* source),
	void (*hPacket)(void* packet, int dataLength, ENetPeer* source), void (*tEvent)())
{
	HandleConnect = hConnect;
	HandleDisconnect = hDisconnect;
	HandlePacket = hPacket;
	TimedEvent = tEvent;
}

bool NetServer::SendMessage(const char* message, ENetPeer* peer)
{
	ENetPacket* packet = CreatePacket(message, strlen(message)+1);
	if (!packet)
		return false;

	// Send the packet

	if (enet_peer_send(peer, DEFAULT_CHANNEL, packet) < 0)
	{
		// Failed to send
		return false;
	}
	return true;
}

bool NetServer::SendData(const void* data, int dataLength, ENetPeer* dest)
{
	ENetPacket* packet = CreatePacket(data, dataLength);

	if(!packet) return false;

	if (enet_peer_send(dest, DEFAULT_CHANNEL, packet) < 0) return false;
	return true;
}

bool NetServer::Broadcast(const char* message) {
	bool success = true;
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients.at(i)->state != ST_EMPTY) {
			if (!SendMessage(message, clients.at(i)->peer))
				success = false;
		}
	}
	return success;
}

ENetPacket* NetServer::CreatePacket(const void* data, int dataLength)
{
	ENetPacket* packet = enet_packet_create(data, dataLength, ENET_PACKET_FLAG_RELIABLE);
	return packet;
}

bool NetServer::SetupServer(int port = 4000)
{
	address.host = ENET_HOST_ANY;
	address.port = port;

	server = enet_host_create(&address, MAX_CLIENTS+1, NUM_CHANNELS, 0, 0);

	lastUpdateMaster = 0;
	RegisterMaster();

	printf("Server Setup\n");

	return (server != NULL);
}


bool NetServer::ServerStep()
{
	// use lastTic to keep track of how many tics per second we have; control "tics per second"
	// perform any periodic, timed events here

	ENetEvent event;
	int status = enet_host_service(server, &event, HOST_TIMEOUT);

	if (status < 0) return false; // ERROR happened! Server shut down!	

	TimedEvent();

	if (status == 0) return true; // nothing to do, try again later

	switch(event.type)
	{
	case ENET_EVENT_TYPE_CONNECT:
		{
			printf ("A new client (%d) connected from %x:%u.\n", 
				event.peer,
				event.peer -> address.host,
				event.peer -> address.port);

			client *c = new client;
			char hostname[256] = "error";
			enet_address_get_host_ip(&event.peer->address, hostname, 256);
			printf("%s connected\n", hostname);

			if (event.peer != master)
			{
				c->hostname = string(hostname);
				c->peer = event.peer;
				c->state = ST_AUTH;

				// associate peer with the client object for them
				event.peer->data = c;

				// Call HandleConnect method of the game logic
				// if it returns false, then that means connection denied, otherwise specifies state client is in
				// also add to list of clients
				if (HandleConnect(event.peer)) {
					bool client_added = false;
					for (size_t i = 0; i < clients.size(); i++) {
						if (clients.at(i)->state == ST_EMPTY) {
							clients.at(i) = c;
							client_added = true;
							break;
						}
					}
					if (!client_added)
						clients.push_back(c);
				} else {
					enet_peer_disconnect(event.peer, 121);
				}
			} else {
				MasterCheckin();
			}
			break;
		}

	case ENET_EVENT_TYPE_DISCONNECT:
		{
			if (event.peer != master) {
				printf ("%d disconnected.\n", event.peer);

				// Call HandleDisconnect method some form of id for the connection
				HandleDisconnect(323, event.peer);

				// free up this client slot for reuse
				((client*) event.peer->data)->state = ST_EMPTY;
			} else {
				printf ("Master timed out.\n");
				master = 0;
			}
			break;
		}
	case ENET_EVENT_TYPE_RECEIVE:
		{
			char hostname[256] = "error";
			enet_address_get_host_ip(&event.peer->address, hostname, 256);
			printf("receiving packet.\n");
			printf ("A packet of length %u ",
				event.packet -> dataLength);
			printf("containing %s ",
				event.packet -> data);
			printf("was received from %s ",
				hostname);
			printf("on channel %u.\n",
				event.channelID);
			/*printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
			event.packet -> dataLength,
			event.packet -> data,
			((client*)event.peer -> data)->hostname.c_str(),
			event.channelID);*/

			client *c = (client*) event.peer->data;

			// Call Packet Handler to deal with packets, and send a response
			HandlePacket(event.packet->data, event.packet->dataLength, event.peer);

			if (event.packet->referenceCount == 0) enet_packet_destroy(event.packet);
			break;
		}
	default:
		break;
	}
	enet_host_flush(server);
	return true;
}

void NetServer::RunDedicated()
{
	printf("Running as dedicated server\n");

	// server loop
	printf("Server loop started\n");
	while (1) {
		ServerStep();	// queue up packets
	}
}

bool NetServer::RegisterMaster()
{
	//int curTime = (int) enet_time_get();
	//if (!lastUpdateMaster || (curTime - lastUpdateMaster > MASTER_UPDATE_TIME*60*1000))
	//{

	ENetAddress addr;
	addr.port = 5000;
	if (enet_address_set_host(&addr, "acs-cseb220-04") < 0) return false;

	ENetPeer* peer = enet_host_connect(server, &addr, 2, 0);
	if (peer == NULL) return false;
	master = peer;

	//	lastUpdateMaster = curTime;
	//	return true;
	//} else {
	//	return false;
	//}
	return true;
}

bool NetServer::UnregisterMaster()
{
	enet_peer_disconnect(master,1);
	master = NULL;
	lastUpdateMaster = 0;
	return false;
}

void NetServer::MasterCheckin()
{
	if (master)
	{
		SendMessage("R", master);
	}
}

void NetServer:: MasterCheckout()
{
	if (master)
	{
		SendMessage("U", master);
	}
}

bool NetServer::Initialize()
{
	int ret = enet_initialize();

	if (!ret) {
		// something failed...
	} else {
		atexit(enet_deinitialize);
	}
	return ret!=0;
}

void NetServer::Deinitialize()
{
	enet_deinitialize();
}