#include <string>
#include <vector>
#include "enet/enet.h"

using namespace std;

enum
{
	ST_EMPTY,
	ST_AUTH,
	ST_SPEC,
	ST_READY,
	ST_PLAY
};

struct client
{
	int type;
	int state;
	ENetPeer *peer;
	string hostname;
	void *info;
};



class NetServer
{
	ENetAddress address;
	ENetHost * server;
	ENetPeer * master;
	vector<client*> clients;
	int lastTic;
	int lastUpdateMaster;

public:
	static const int DEFAULT_CHANNEL = 0;
	static const int MAX_CLIENTS = 32; // max of 32 clients
	static const int NUM_CHANNELS = 2; // 2 channels
	static const int HOST_TIMEOUT = 25; // small numbers hurt performance, large numbers hurt latency
	static const int MASTER_UPDATE_TIME = 5; // update every 5 minutes

	// Create new server
	//NetServer();

	//~NetServer();

	// Registers event handlers
	void RegisterHandlers(bool (*HandleConnect)(ENetPeer* source),
			void (*HandleDisconnect)(int reason, ENetPeer* source),
			void (*HandlePacket)(void* packet, int dataLength, ENetPeer* source), void (*TimedEvent)());

	// Send a message to a peer
	// message is the contents to send them
	// peer is the person to send the message to
	bool SendMessage(const char* message, ENetPeer* peer);

	// Send a message to everyone connected
	bool Broadcast(const char* message);

	// Send data to a client
	bool SendData(const void* data, int dataLength, ENetPeer* dest);

	// Create ENet packet from a message
	ENetPacket* CreatePacket(const void* message, int dataLength);

	// Start listening for connections
	bool SetupServer(int port);

	// Run tick of the server
	bool ServerStep();

	// Run as a dedicated server
	void RunDedicated();

	// List with master server
	bool RegisterMaster();

	// Delist with master server
	bool UnregisterMaster();

	// Send checkin packet to master
	void MasterCheckin();

	// Send checkout packet to master
	void MasterCheckout();

	// Initialize ENet
	bool Initialize();

	// Deinitialize ENet
	void Deinitialize();

private:
	bool (*HandleConnect)(ENetPeer* source);
	void (*HandleDisconnect)(int reason, ENetPeer* source);
	void (*HandlePacket)(void* packet, int dataLength, ENetPeer* source);
	void (*TimedEvent)();
};