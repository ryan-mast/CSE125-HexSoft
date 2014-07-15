// #define WIN32

#include <string>
#include <vector>
#include "enet/enet.h"

using namespace std;

enum {
	ST_EMPTY,
	ST_REG
};

struct server
{
	int state;
	char* name;
	char* address;
	int port;
	int lastUpdate;
};

ENetAddress address;
ENetHost * host;
string server_list;
vector<server*> servers;
int lastListRefresh;

static const int MAX_CLIENTS = 32;
static const int MASTER_TIMEOUT = 50;
static const int DEFAULT_CHANNEL = 0;
static const int NUM_CHANNELS = 1;
static const int LIST_TIMEOUT = 5;

void RecreateList() {
	server_list.clear();
	int curTime = (int) enet_time_get();
	bool first_add = true;
	char port[33];
	for (int i = 0; i < servers.size(); i++) {
		if (servers.at(i)->state == ST_REG)
		{
			if ((curTime - servers.at(i)->lastUpdate) > LIST_TIMEOUT*60*1000)
			{
				servers.at(i)->state = ST_EMPTY;
			}
			else 
			{
				if (!first_add) {
					server_list.append(",");
				}
				server_list.append(servers.at(i)->address);
				server_list.append(":");
				snprintf(port, 32, "%d", servers.at(i)->port);
				server_list.append(port);
				first_add = false;
			}
		}
	}

	lastListRefresh = curTime;
}

ENetPacket* CreatePacket(const char* message)
{
	ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
	return packet;
}

bool SendMessage(const char* message, ENetPeer* peer)
{
	ENetPacket* packet = CreatePacket(message);
	if (!packet)
		return false;

	// Send the packet
	if (enet_peer_send(peer, DEFAULT_CHANNEL, packet) < 0)
	{
		// Failed to send
		return false;
	}
}

bool SetupServer(int port = 4000)
{
	address.host = ENET_HOST_ANY;
	address.port = port;

	host = enet_host_create(&address, MAX_CLIENTS, NUM_CHANNELS, 0, 0);

	printf("Server Setup\n");

	return (host != NULL);
}

bool ServerStep()
{
	// use lastTic to keep track of how many tics per second we have; control "tics per second"
	// perform any periodic, timed events here

	ENetEvent event;
	int status = enet_host_service(host, &event, MASTER_TIMEOUT);

	if (status < 0) return false; // ERROR happened! Server shut down!

	if (status == 0) return true; // nothing to do, try again later

	switch(event.type)
	{
	case ENET_EVENT_TYPE_CONNECT:
		{
			printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);

			char hostname[256] = "error";
			enet_address_get_host_ip(&event.peer->address, hostname, 256);
			printf("%s connected\n", hostname);
			break;
		}

	case ENET_EVENT_TYPE_DISCONNECT:
		{
			printf ("%s disconected.\n", event.peer -> data);
			break;
		}
	case ENET_EVENT_TYPE_RECEIVE:
		{
			printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);
			
			if (event.packet->dataLength > 0) {
				switch (event.packet->data[0]) {
				case 'L':
					{
						// Requested the list; send it to them
						SendMessage(server_list.c_str(), event.peer);
						printf("Sent: %s\n", server_list.c_str());
						break;
					}
				case 'R':
					{
						// Registering with the master, add them to our list!
						char hostname[256];
						char port[33];
						strcpy(hostname, "error");
						enet_address_get_host_ip(&event.peer->address, hostname, 256);
						//if (event.packet->dataLength >= 5) {
						//	port = *((int*)(&event.packet->data[1]));
						//}

						server* this_server;
						bool found = false;
						int first_empty_slot = -1;
						int i;

						// Search server list to see if they already exist
						for (i = 0; i < servers.size(); i++) {
							if (first_empty_slot == -1 && servers.at(i)->state == ST_EMPTY)
							{
								printf("Found empty slot: %d\n", i);
								first_empty_slot = i;
							}
							if (strcmp(servers.at(i)->address, hostname)==0 && servers.at(i)->port == event.peer->address.port)
							{
								printf("Found server: %s, %s\n", hostname, servers.at(i)->address);
								found = true;
								this_server = servers.at(i);
								break;
							}
						}

						if (!found) {
							if (first_empty_slot != -1)
							{
								printf("First empty slot: %d\n", first_empty_slot);
								this_server = servers.at(first_empty_slot);
							} else {
								this_server = (server*)malloc(sizeof(server));
								this_server->address = (char*)malloc(256);
								servers.push_back(this_server);
							}
							strcpy(this_server->address, hostname);
							printf("%s added\n", this_server->address);
							this_server->port = (int) event.peer->address.port;
							this_server->state = ST_REG;

							if (servers.size() > 1)
								server_list.append(",");
							server_list.append(this_server->address);
							server_list.append(":");
							snprintf(port, 32, "%d", servers.at(i)->port);
							server_list.append(port);
						}

						this_server->lastUpdate = (int) enet_time_get();

						printf("%s\n", server_list.c_str());
						break;
					}
				case 'U':
					// Unregistering the server
					char * hostname = (char*)malloc(256);
						int port = 4000;
						strcpy(hostname, "error");
						enet_address_get_host_ip(&event.peer->address, hostname, 256);
						//if (event.packet->dataLength >= 5) {
						//	port = *((int*)(&event.packet->data[1]));
						//}

						server* this_server;
						bool found = false;

						// Search server list to see if they already exist
						for (int i = 0; i < servers.size(); i++) {
							if (strcmp(servers.at(i)->address, hostname))
							{
								found = true;
								servers.at(i)->state = ST_EMPTY;
								break;
							}
						}

						RecreateList();
						printf("%s\n", server_list.c_str());
						break;
				}
			}

			if (event.packet->referenceCount == 0) enet_packet_destroy(event.packet);
			break;
		}
	default:
		break;
	}
	enet_host_flush(host);
}

bool Initialize()
{
	server_list = "";
	lastListRefresh = (int) enet_time_get();

	int ret = enet_initialize();
	
	if (!ret) {
		// something failed...
	} else {
		atexit(enet_deinitialize);
	}
	return ret;
}

void Deinitialize()
{
	enet_deinitialize();
}

int main (int argc, char ** argv)
{
	Initialize(); // Should only be called once at most
	SetupServer(5000); // Can be called multiple times with different ports... say if you wanted to host multiple games with a single server
	
	int curTime;

	for(;;)
	{
		ServerStep(); // Serv the list!
		curTime = (int) enet_time_get();
		if ((servers.size() > 0) && ((curTime - lastListRefresh) > 60000))
			RecreateList();
	}
}
