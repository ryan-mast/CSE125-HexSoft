#ifndef TILEBOARD_H
#define TILEBOARD_H

#include <stdlib.h>
#include <iostream>
#include <iomanip> 
#include <string>
#include <sstream>
#include <Windows.h>
#include <atlstr.h>
#include <time.h>
#include "Character.h"
#include "Player.h"

using namespace std;
//extern MatrixTransform * world;
class TileBoard {
	static const int MAX_PLAYERS = 4;

	
	//Character*** board_tiles;
	vector<Player*> players;
	int lastPlayer;
	int size;
	clock_t startTime;

	void initializeArmy(Player*);
	void initBoard(int);
public:
	Character*** board;
	TileBoard();
	TileBoard(int);
	~TileBoard();

	string printBoard();
	
	// game status functions
	clock_t getStartTime();
	bool startBoard();
	bool isStarted();
	bool endBoard();
	bool isEnded();

	// player functions
	Player* addPlayer();
	Player* addPlayer(int);
	void removePlayer(int);
	int getPlayerIndex(Player*);
	Player* getPlayerById(int);
	Player* getPlayerByAddress(int);
	int getPlayerCount();
	int getLastPlayer();
	vector<Player*> getReadyPlayers();
	vector<Player*> getGamePlayers();
	bool isFull();
	bool isWinner();
	Player* getWinner();
	bool checkEmpty(int, int);

	// character functions	
	bool addCharacter (Player*, Type, bool);
	void setCharacterTile(Character*, int, int);	
	bool setCharacterDest(Character*, int, int);
	vector<Character*> getDeadChars();
	bool updateCharacterLocation(char*);
	bool updateLocationsOnClient(char*);

	// game update functions
	void minusCooldown();
	void incHexBank();
};

#endif