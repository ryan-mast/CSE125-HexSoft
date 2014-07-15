#include "enet/enet.h"

using namespace std;

struct server_list_item {
	char * address;
	int port;
	char * name;
};


class NetClient
{
public:
	static const int CHANNEL_COUNT = 2;
	static const int DEFAULT_CHANNEL = 0;

	// Create client and associate with game logic object
	NetClient();

	virtual ~NetClient();

	// Registers event handlers
	void RegisterHandlers(bool (*HandleConnect)(),
			void (*HandleDisconnect)(int reason),
			void (*HandlePacket)(void* packet, int dataLength));

	// Create conenction to a server
	bool Connect(const char* address, const int port);

	// Disconnect
	void Disconnect(int reason);

	// Ask for queued messages (must call once each frame)
	int Poll();

	// Send any outgoing messages in the queue
	void Flush();


	// Send message to server
	bool SendMessage(const char* message);

	// Send data to the server
	bool SendData(const void* data, int dataLength);

	// Fetch server list
	bool FetchServerList();

	// Server list received
	bool ReceivedServerList();

	// Server list get item
	server_list_item* GetNextServer();

	// Create a packet
	ENetPacket* CreatePacket(const void* data, int dataLength);

	// Initialize ENet
	bool Initialize();

	// Deinitialize ENet
	void Deinitialize();

private:
	ENetHost* host;
	ENetPeer* server;
	ENetPeer* master;
	bool (*HandleConnect)();
	void (*HandleDisconnect)(int reason);
	void (*HandlePacket)(void* packet, int dataLength);
	char* server_list;
	char* curServer;
	server_list_item* server_item;
	bool first_server;
	bool server_list_received;
};