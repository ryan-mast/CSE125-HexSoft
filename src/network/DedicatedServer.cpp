#define WIN32

#include <vector>
#include <string>
#include <queue>
#include "NetServer.h"
#include "../game/Tileboard.h"

NetServer server;
TileBoard* board;
int lastCooldownUpdate;
int lastHexbankUpdate;
int lastLocationUpdate;
int startGameTimer;

void Broadcast(const char* s) {
	server.Broadcast(s);
}

void Broadcast(const char* s, bool debug) {
	server.Broadcast(s);
	if (debug)
		printf("Broadcasting %s.\n",s);
}

void PrintBoard() {
	printf ("  %s", board->printBoard().c_str());
}

void TimedEvent() {
	int curTime = enet_time_get();

	// if game has not started
	if (!board->isStarted()) {
		// check to see if more than one player and if all players are ready.
		if (board->getPlayerCount() > 1 && board->getReadyPlayers().size() == board->getPlayerCount()) {
			if (startGameTimer == -1) {
				// start game timer
				startGameTimer = curTime;
				printf("Game will start in 1 seconds.\n");
			}
			// check to see if 10 seconds have elapsed and start game.
			else if (curTime - startGameTimer >= 1000) {
				if (board->startBoard()) {
					printf ("GAME HAS STARTED.\n");
					Broadcast ("SG\0");
				} else
					printf ("Error starting game.\n");
			}
		} else {
			if (startGameTimer!=-1)
				printf("Start game timer reset.\n");
			// reset timer if players not all ready.
			startGameTimer = -1;
		}

	} 
	// if game has started and not ended
	else if (!board->isEnded()) {	
		// check for a winner
		if (board->isWinner()) {
			board->endBoard();
			printf ("Game ended. Winner is Player %d.\n",board->getWinner()->getId());
			Broadcast ("W");
		}
		else {
			if (curTime - lastHexbankUpdate > 5000)
			{
				board->incHexBank(); // increment player hexbanks every 5 secs
				lastHexbankUpdate = curTime;
				printf ("Hexbanks incremented.\n");
				Broadcast ("TH");
			}
			if (curTime - lastCooldownUpdate > 125)
			{
				board->minusCooldown(); // run every tic.
				lastCooldownUpdate = curTime;
				Broadcast ("TC");
			}
			if (curTime - lastLocationUpdate > 125)
			{
				char packet[256];
				if (board->updateCharacterLocation(packet)) {
				Broadcast(packet);
				}
			}
		}
	}
}

void HandlePacket(void* packet, int dataLength, ENetPeer* source)
{
	/* store packet into queue */
	char* data = (char*) packet;
	char response[256];
	int i, x, y;
	Player * p;
	Character * c;

	memcpy(response, packet, dataLength);
	switch (data[0]) {
	case 'C':	/* a client has connected and requesting id */
		if (board->isStarted()) {
			printf("  Game has already started. Unable to add new player.\n");
			return;
		}
		if (board->isFull()) {
			printf("  Board full. Unable to add new player.\n");
			return;
		}
		if (data[1] == 'X') {
			printf("  Adding player.\n");
			p = board->addPlayer((int)source);
			printf("  Player at %d added. Current player count is %d. Last player is %d.\n", source, board->getPlayerCount(),board->getLastPlayer());
			printf("%s\n",board->printBoard().c_str());
			response[1] = '0' + board->getLastPlayer();
			Broadcast(response);
		}
		break;
	case 'S':	/* received a status packet */
		switch (data[1]) {
		case 'R':	/* received ready packet */
			// 'X' means a request for all ready players.
			if (data[2] == 'X') {
				vector<Player*> rp = board->getReadyPlayers();
				for (size_t i=0; i<rp.size(); i++) {
					response[2] = '0' + rp[i]->getId();
					Broadcast(response);
				}
			}
			// otherwise get player ready request.
			else {
				i = data[2]-'0';
				if (p = board->getPlayerById(i)) {
					p->isReady(true);				
					printf ("  Player %d set as ready.\n",i);					
					Broadcast(response);
				}
			}
			break;
		case 'N':	/* set player as not ready */
			i = data[2]-'0';
			if (Player* p = board->getPlayerById(i)) {
				p->isReady(false);				
				printf ("  Player %d set as not ready.\n",i);				
				Broadcast(response);
			}
			break;
		default:
			// refresh board output
			PrintBoard();
			break;
		}
		break;
	case 'G':		/* game packet */
		if ((p = board->getPlayerById(data[2]-'0')) == NULL) {
			printf("player %d not found!\n", data[2]-'0');
			return;
		}
		switch (data[1]) {
		case 'B':	/* bought character */
			switch (data[3]) {
				// figure out character type
			case 'S':
				if (board->addCharacter(p,soldier,true)) {
					printf ("  Player %d bought a soldier.\n",p->getId());
					Broadcast(response);
				} else {
					printf ("  Player %d could not buy new character.  Possibly because spawn location is not empty\n", p->getId());
				}
				break;
			case 'A':
				if (board->addCharacter(p,archer,true)) {
					printf ("  Player %d bought an archer.\n",p->getId());
					Broadcast(response);
				} else {
					printf ("  Player %d could not buy new character.  Possibly because spawn location is not empty\n", p->getId());
				}
				break;
			case 'P':
				if (board->addCharacter(p,paladin,true)) {
					printf ("  Player %d bought a paladin.\n",p->getId());
					Broadcast(response);
				} else {
					printf ("  Player %d could not buy new character.  Possibly because spawn location is not empty\n", p->getId());
				}
				break;
			case 'H':
				if (board->addCharacter(p,healer,true)) {
					printf ("  Player %d bought a healer.\n",p->getId());
					Broadcast(response);
				} else {
					printf ("  Player %d could not buy new character.  Possibly because spawn location is not empty\n", p->getId());
				}
				break;
			}
			break;
		case 'M':	
			/* move character			
			 * packet format:			G | M | playerID | character ID | x-coord | y-coord
			 */
			x = data[4]-'0';
			y = data[5]-'0';

			if ((c = p->getCharacter(data[3]-'0')) == NULL) {
				printf("  MOVE - could not find player %d's character %d\n", p->getId(), data[3]-'0');
			}

			if (!board->checkEmpty(x, y)) {
				printf("  MOVE - could not set player %d's character %d's destination to %d, %d because it is not empty\n", 
					p->getId(), c->getId(), x, y);
				return;
			}

			if (board->setCharacterDest(c, x, y)) {
				coord dcoord = c->getDestCoord();
				printf("  MOVE - Setting destination for player %d's character %d to %d, %d.\n", 
					p->getId(), c->getId(), dcoord.getX(), dcoord.getY());
				Broadcast(response);
			} else {
				printf("  MOVE - problem setting player %d's character %d's destination\n", p->getId(), c->getId());
			}

			break;
		default:
			break;
		}
		printf("%s\n",board->printBoard().c_str());
		break;
	case 'E':		/* error message */
		break;
	default:
		break;
	}	
}

bool HandleConnect(ENetPeer* source)
{
	// Yay, someone joined us!
	return true;
}

void HandleDisconnect(int reason, ENetPeer* source)
{	
	Player* p = board->getPlayerByAddress((int)source);
	ostringstream output;
	if (p != NULL) {
		if (board->isStarted()) {
			// mark player as lost
			p->isDefeated(true);
			output << "SD" << p->getId() << "\0";
			Broadcast(output.str().c_str(),true);
			printf("Player %d disconnected. Signalled as lost.\n",p->getId());
		} else {
			// remove player from play list
			board->removePlayer(p->getId());
			printf("Player %d disconnected. Removed from player list.\n",p->getId());
		}
		PrintBoard();
	}
	// Aw, someone left the game.
}

int main (int argc, char ** argv)
{
	startGameTimer = -1;
	if (board == NULL) {
		board = new TileBoard();
	}
	lastHexbankUpdate = enet_time_get();
	lastCooldownUpdate = enet_time_get();
	lastLocationUpdate = enet_time_get();
	server.Initialize(); // Should only be called once at most
	server.RegisterHandlers(&HandleConnect, &HandleDisconnect, &HandlePacket, &TimedEvent); // registers the event handlers
	server.SetupServer(4000); // Can be called multiple times with different ports... say if you wanted to host multiple games with a single server
	server.RunDedicated(); // Run it as a standalone dedicated server (Note: Does not return!)
}