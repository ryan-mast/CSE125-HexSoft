#include "TileBoard.h"
#include "../graphics/graphicsStuff.h"

// Initializes the board
void TileBoard::initBoard(int s) {
	startTime = -1;
	size = s;
	lastPlayer = 0;

	//dynamically allocate array
	board = (Character***) malloc (size * sizeof(Character**));
	//board_tiles = (Character***) malloc (size * sizeof(Character**));
	for(int i = 0; i < size; i++) {
		board[i] = (Character**) malloc (size * sizeof(Character*));
		//board_tiles[i]=(Character**) malloc (size * sizeof(Character*));
		for (int j=0; j<size; j++)
		{
			
			board[i][j]= NULL;

			//patrick's Scenegraph
			//boardtiles stores the tile characters
			//for storeing tile name
			/*stringstream os;
			os << "tile"<<i<<","<<j;
			string name = os.str();
			board_tiles[i][j]= new Character();
			board_tiles[i][j]->node= new MatrixTransform(name);
			board_tiles[i][j]->node->translate(TILE_LENGTH*i,0,TILE_LENGTH*j);
			world->addChild(board_tiles[i][j]->node);
			board_tiles[i][j]->node->addChild(tile);
			*/
		}
	}
}

// constructor classes
TileBoard::TileBoard() {
	initBoard(10);
}

TileBoard::TileBoard(int s) {
	initBoard(s);
}

// destructor class
TileBoard::~TileBoard() {
	for(int i = 0; i < size; i++) {		
		for (int j=0; j<size; j++)
			delete board[i][j];
		delete board[i];
	}
	delete board;
	players.clear();
}

// Returns a string of the board with some player details.
string TileBoard::printBoard() {

	// print out y coordinates up top
	ostringstream output;
	output << "\n\tGAME STARTED: " << startTime << "\n";
	output << "\t  ";
	for (int i = 0; i < 10; i++) {
		output << "  " << i << "   ";
	}
	output << '\n';
	output << "\t |-----------------------------------------------------------|\n";
	for (int x = size-1; x>-1 ; x--) {
		// print out x coordinate on the left
		ostringstream o1,o2;
		o1 << '\t' << x;
		o2 << '\t' << ' ';
		// print out board		
		for (int y = 0; y < 10; y++) {
			o1 << "|";
			o2 << "|";
			if (board[x][y] == NULL) {
				o1 << setw(5) << " ";
				o2 << setw(5) << " ";
			} else {
				o1 << board[x][y]->getParent();
				switch(board[x][y]->getType()) {
				case archer:
					o1 << "a";					
					break;
				case healer:
					o1 << "h";
					break;
				case king:
					o1 << "k";
					break;
				case paladin:
					o1 << "p";
					break;
				case soldier:
					o1 << "s";
					break;
				case null:
					o1 << " ";
					break;
				}
				o1 << board[x][y]->getId() << "." << board[x][y]->getCooldown();
				o2 << setw(2) << board[x][y]->getHealth() << '/' << setw(2) << board[x][y]->getMaxHealth();
			}
		}
		o1 << "|\n";	
		o2 << "|\n";
		output << o1.str() << o2.str();
		output << "\t |-----------------------------------------------------------|\n";
	}	

	// output Player data
	for (size_t i=0; i<players.size(); i++) {
		Player* p = players[i];
		output << "\t" << p->getIndex() << ". Player " << p->getId();
		if (p->isReady()) {
			output << " READY";
			if (p->isDefeated())
				output << " DEFEATED";
		}
	    output << "\n";
		output << "\t  army: " << p->getArmySize() << "/" << p->getMaxArmy() << "\n";
		output << "\t  hexbank: " << p->getHexbank() << " hexcoins\n";
	}
	return output.str();
}

// Returns board start time. Returns -1 if board has not been
// started yet.
clock_t TileBoard::getStartTime() {
	return startTime;
}

// Sets start time of the board and initializes player armies.
// Returns true if successful.
bool TileBoard::startBoard(){
	if (startTime == -1) {
		startTime = clock();
		for (size_t i=0; i<players.size(); i++) {
			initializeArmy(players[i]);
		}
		return true;
	}
	return false;
}

// Returns true if game has started.
bool TileBoard::isStarted(){
	return startTime>-1;
}

// Ends the game by setting the startTime to 0. Returns 
// true if successful.
bool TileBoard::endBoard(){
	if (startTime > 0) {
		startTime = 0;
		return true;
	}
	return false;
}

// Returns true if game has ended.
bool TileBoard::isEnded(){
	return startTime==0;
}

// Adds a new player to the board.
Player* TileBoard::addPlayer() {
	Player* p = new Player(++lastPlayer);
	players.push_back(p);
	p->setIndex(players.size());
	return p;
};

// Adds a new player to the board with given pointer address a.
Player* TileBoard::addPlayer(int a) {
	Player* p = new Player(++lastPlayer);
	p->setAddress(a);
	players.push_back(p);
	p->setIndex(players.size());
	return p;
};

// Removes a player from the board given a PlayerID id.
void TileBoard::removePlayer(int id) {
	for (size_t i=0; i<players.size(); i++) {
		if (players[i]->getId() == id) {
			players.erase(players.begin()+i);
		}
	}
};

// Initializes Player p's army in his respective corner.
void TileBoard::initializeArmy(Player* p){
	Character* c;
	int xdiff, ydiff;

	// spawn king in spawn tile
	c = p->addArmy(king,false);
	setCharacterTile(c, p->getSpawnX(), p->getSpawnY());

	if (p->getSpawnX() == 0)
		xdiff = 1;
	else
		xdiff = -1;
	if (p->getSpawnY() == 0)
		ydiff = 1;
	else
		ydiff = -1;

	// adding 3 soldiers in tiles around spawn tile
	c = p->addArmy(soldier,false);
	setCharacterTile(c, p->getSpawnX(), p->getSpawnY()+ydiff);
	c = p->addArmy(soldier,false);
	setCharacterTile(c, p->getSpawnX()+xdiff, p->getSpawnY());
	c = p->addArmy(soldier,false);
	setCharacterTile(c, p->getSpawnX()+xdiff, p->getSpawnY()+ydiff);
}

// Returns the pointer to the player with PlayerID id. Returns NULL if
// player not found.
Player* TileBoard::getPlayerById(int id) {
	for (size_t i=0; i<players.size(); i++) {
		if (players[i]->getId() == id)
			return players[i];
	}
	return NULL;
}

// Returns the pointer to the player with PlayerAddress address. Returns NULL if
// player not found.
Player* TileBoard::getPlayerByAddress(int address) {
	for (size_t i=0; i<players.size(); i++) {
		if (players[i]->getAddress() == address)
			return players[i];
	}
	return NULL;
}

// Returns current number of players on the board.
int TileBoard::getPlayerCount() {
	return players.size();
}

// Returns the last ID of a player connected to the board.
int TileBoard::getLastPlayer() {
	return lastPlayer;
}

// Returns a vector of players that are ready to play
vector<Player*> TileBoard::getReadyPlayers() {
	vector<Player*> readyPlayers;
	for (size_t i=0; i<players.size(); i++) {
		if (players[i]->isReady())
			readyPlayers.push_back(players[i]);
	}
	return readyPlayers;
}

// Returns a vector of players that are still in game
vector<Player*> TileBoard::getGamePlayers() {
	vector<Player*> gamePlayers;
	for (size_t i=0; i<players.size(); i++) {
		if (!players[i]->isDefeated())
			gamePlayers.push_back(players[i]);
	}
	return gamePlayers;
}

// Returns true if there are already 4 players on the board.
bool TileBoard::isFull() {
	return getPlayerCount()>=MAX_PLAYERS;
}

// Returns true if there is only one winner left.
bool TileBoard::isWinner() {
	return getGamePlayers().size() == 1;
}

// Returns Player* to winner. Returns NULL if no winner yet.
Player* TileBoard::getWinner() {
	if (isWinner())
		return getGamePlayers()[0];
	else
		return NULL;
}

// checks if a specific tile is empty
bool TileBoard::checkEmpty(int x, int y) {
	if (board[x][y] == NULL) {
		return true;
	} else {
		return false;
	}
}

// Adds a character of Type t to the army of Player p and
// spawn corner if no character is on the tile. Returns true
// if successful.
bool TileBoard::addCharacter (Player* p, Type t, bool buy) {
	Character* c = p->addArmy(t, buy);

	if (c == NULL)
		return false;
	if (board[p->getSpawnX()][p->getSpawnY()] != NULL) {
		p->delArmyChar(c->getId());
		return false;
	}
	
	setCharacterTile(c, p->getSpawnX(), p->getSpawnY());
	return true;
}

// Sets the location of a Character c in tile (x,y).
void TileBoard::setCharacterTile(Character* c, int x, int y) {
	if (c->isInitialized()) {
		coord temp = c->getCurrCoord();
		board[temp.getX()][temp.getY()] = NULL;
	}
	c->setCurrCoord(x,y);	
	board[x][y] = c;
}

bool TileBoard::setCharacterDest(Character * c, int x, int y) {
	if (c == NULL || x >= size || y >= size) {
		return false;
	}
	if (board[x][y] == NULL) {
		c->setDestCoord(x, y);
		return true;
	}
	return false;
}

// Returns a vector of characters that are dead
vector<Character*> TileBoard::getDeadChars() {
	vector<Character*> deadChars;
	for(int i = 0; i<size; i++)
		for (int j=0; j<size; j++)
			if (!board[i][j]->isDead())
			deadChars.push_back(board[i][j]);
	return deadChars;
}

// Run through players to update characters' cooldown timers.
void TileBoard::minusCooldown() {
	for (size_t i=0; i<players.size(); i++)
		players[i]->minusCooldown();
}

// Run through players to increment their hexbank.
void TileBoard::incHexBank() {
	for (size_t i=0; i<players.size(); i++)
		players[i]->incHexbank(1);
}

bool TileBoard::updateCharacterLocation(char * packet) {
	vector<Character*> characters;
	Player *p;
	Character *c;
	coord currLocation;
	bool oneUpdated = false;

	*packet++ = 'L';	// initial stuff for packet

	for (size_t i = 0; i < players.size(); i++) {
		p = players[i];
		characters = p->getArmy();
		for ( size_t j = 0; j < characters.size(); j++) {
			c = characters[j];

			// add to packet if characters location is updated
			if (c->updateLocation(board)) {
				oneUpdated = true;
				currLocation = c->getCurrCoord();
				*packet++ = p->getId();
				*packet++ = c->getId();
				*packet++ = currLocation.getX();
				*packet++ = currLocation.getY();
				printf("character %d moved to %d, %d\n", c->getId(), currLocation.getX(), currLocation.getY());
			}
		}
	}

	*packet = '\0';
	return oneUpdated;
}

bool TileBoard::updateLocationsOnClient(char * packet) {
	Player * player;
	Character * character;
	int x;
	int y;

	while (*packet != '\0') {
		if ((player = getPlayerById(*packet++)) == NULL) {
			return false;
		}
		if ((character = player->getCharacter(*packet++)) == NULL) {
			return false;
		}
		x = *packet++;
		y = *packet++;
		setCharacterTile(character, x, y);
	}
	
	return true;
}