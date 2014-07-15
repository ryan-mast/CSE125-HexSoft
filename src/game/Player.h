#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <stdlib.h>
#include "Character.h"

using namespace std;

class Player {
  int playerId; 
  int playerAddress;
  int playerIndex;
  Character* kingChar;
  // army does not include the king.
  vector<Character*> army; 
  int maxArmy;
  int hexbank;
  int armyCount;
  bool ready;

public:
	Player(int p);
	~Player();
	int getId();
	int getAddress();
	void setAddress(int);
	int getIndex();
	void setIndex(int);

	// army
	Character * addArmy(Type,bool);
	Character * getCharacter(int);
	int getCharacterIndex(int);
	bool delArmyChar(int);
	int getArmySize();
	int getMaxArmy();
	bool isArmyMax();
	int getSpawnX();
	int getSpawnY();
	void minusCooldown();
	vector<Character*> getArmy();
	
	// hexbank
	int getHexbank();
	void incHexbank(int);
	void decHexbank(int);

	// defeated
	bool isDefeated();
	void isDefeated(bool);

	// ready to start game
	bool isReady();
	void isReady(bool);
};

#endif