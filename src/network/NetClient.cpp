#define WIN32

#include <string>
#include "NetClient.h"

using namespace std;

NetClient::NetClient() : host(NULL), server(NULL)
{
}

void NetClient::RegisterHandlers(bool (*hConnect)(),
		void (*hDisconnect)(int reason),
		void (*hPacket)(void* packet, int dataLength))
{
	HandleConnect = hConnect;
	HandleDisconnect = hDisconnect;
	HandlePacket = hPacket;
}

NetClient::~NetClient()
{
	if (host && server)
	{
		// disconnect us if this hasn't already been done
		enet_peer_disconnect_now(server, 1234);
		enet_host_destroy(host);

		host = NULL;
		server = NULL;
	}
}

bool NetClient::Connect(const char* address, int port)
{
	if (!server)
	{
		if (!host) {
			ENetHost* h = enet_host_create(NULL, 2, CHANNEL_COUNT, 0, 0);
			if (!h) return false;
			host = h;
		}

		ENetAddress addr;
		addr.port = port;
		if (enet_address_set_host(&addr, address) < 0) return false;

		ENetPeer* peer = enet_host_connect(host, &addr, CHANNEL_COUNT, 0);
		if (peer == NULL) return false;

		server = peer;
		return true;
	} else {
		return false;
	}
}

void NetClient::Disconnect(int reason)
{
	if (host && server)
	{
		enet_peer_disconnect_now(server, reason);
		enet_host_destroy(host);

		host = NULL;
		server = NULL;
	}
}

int NetClient::Poll()
{
	int status = 0;
	printf("Want to Poll\n");
	if (host)
	{
		printf("Polling\n");
		ENetEvent event;
		while (status = enet_host_service(host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				{
					char hostname[256] = "error";
					enet_address_get_host_ip(&event.peer->address, hostname, 256);
					//printf("Connected to: %s\n", hostname);

					//SendMessage("hello world");
					//Flush();
					if (event.peer != master) {
						if (HandleConnect) {
							HandleConnect();
						}
					} else {
						// We asked for the list and just got connected, so send our request
						ENetPacket* packet = CreatePacket("L", 2);
						if(packet) {
							enet_peer_send(master, DEFAULT_CHANNEL, packet);
							enet_host_flush(host);
						}
					}
					break;
				}
			case ENET_EVENT_TYPE_DISCONNECT:
				{
					if (HandleDisconnect) {
						HandleDisconnect(event.data);
					}
					break;
				}
			case ENET_EVENT_TYPE_RECEIVE:
				{
					// do some preliminary handling (check the channel and the source (server, or master?)
					// make sure data actually exists
					
					if (event.peer != master) {
						if (HandlePacket) {
							HandlePacket((char*)event.packet->data, event.packet->dataLength);
						}
					} else {
						// We received a server list!
						server_list_received = true;
						if (server_list) {
							free(server_list);
						}
						server_list = (char*)malloc(event.packet->dataLength + 1);
						char* source = (char*)event.packet->data;
						strcpy_s(server_list, sizeof(source), source);
						first_server = true;
					}

					enet_packet_destroy(event.packet);
					break;
				}
			default:
				break;
			}
		}
	}
	return status;
}

void NetClient::Flush()
{
	if (host && server)
		enet_host_flush(host);
}

ENetPacket* NetClient::CreatePacket(const void* data, int dataLength)
{
	ENetPacket* packet = enet_packet_create(data, dataLength, ENET_PACKET_FLAG_RELIABLE);
	return packet;
}

bool NetClient::SendMessage(const char* message)
{
	if (host && server)
	{
		ENetPacket* packet = CreatePacket(message, strlen(message)+1);

		if (!packet) return false;

		if (enet_peer_send(server, DEFAULT_CHANNEL, packet) < 0) return false;
		return true;
	} else {
		return false;
	}
}

bool NetClient::SendData(const void* data, int dataLength)
{
	if (host && server)
	{
		ENetPacket* packet = CreatePacket(data, dataLength);
		
		if(!packet) return false;

		if (enet_peer_send(server, DEFAULT_CHANNEL, packet) < 0) return false;
		return true;
	} else {
		return false;
	}
}

bool NetClient::FetchServerList()
{
	if (!host) {
			ENetHost* h = enet_host_create(NULL, 2, CHANNEL_COUNT, 0, 0);
			if (!h) return false;
			host = h;
		}

	if (!server_item)
	{
		server_item = (server_list_item*) malloc(sizeof(server_list_item));
	}

	if (!master) {
		// create the socket for the master; connect
		ENetAddress addr;
		addr.port = 5000;
		if (enet_address_set_host(&addr, "acs-cseb220-04") < 0) return false;

		master = enet_host_connect(host, &addr, CHANNEL_COUNT, 0);
		if (master == NULL) return false; // packet will get sent on first connect
	} else {
		ENetPacket* packet = CreatePacket("L", 2);
		if(!packet) return false;
		if (enet_peer_send(server, DEFAULT_CHANNEL, packet) < 0) return false;
	}
	return true;
}

bool NetClient::ReceivedServerList()
{
	return server_list_received;
}

server_list_item* NetClient::GetNextServer()
{
	if (first_server) {
		curServer = strtok(server_list,",:");
		printf("%s\n", curServer);
	}
	if (curServer != NULL && !first_server) {
		curServer = strtok (NULL, ",:");
	}
	if (curServer)
	{
		if (curServer != NULL)
		{
			server_item->address = (char*) malloc(strlen(curServer) + 1);
			strcpy_s(server_item->address, sizeof(curServer), curServer);
		}

		curServer = strtok(NULL,",:");
		if (curServer != NULL)
		{
			server_item->port = atoi(curServer);
		}
	}
	first_server = false;
	return server_item;
}

bool NetClient::Initialize()
{
	int ret = enet_initialize();
	
	if (!ret) {
		// something failed...
	} else {
		atexit(enet_deinitialize);
	}
	return ret;
}

void NetClient::Deinitialize()
{
	enet_deinitialize();
}
