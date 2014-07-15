#include "Player.h"

// constructor class
Player::Player(int p) {
	playerId = p;
	maxArmy = 10;
	hexbank = 0;
	armyCount = 0;
	kingChar = NULL;
	ready = false;
}

// destructor class
Player::~Player() {
	delete kingChar;
	army.clear();
}

// Returns the PlayerID.
int Player::getId() {
	return playerId;
}

// Returns the Player address.
int Player::getAddress() {
	return playerAddress;
}

// Sets the Player address.
void Player::setAddress(int a) {
	playerAddress = a;
}

// Returns the Player index.
int Player::getIndex() {
	return playerIndex;
}

// Sets the Player index.
void Player::setIndex(int i) {
	playerIndex = i;
}

// Inserts a Character of Type t and returns a pointer to the object. ID is 
// generated in the format Parent*1000 + CharID. If Character unable to be added,
// function returns NULL.
Character * Player::addArmy(Type type, bool buy) {
	int charid = getId()*1000 + ++armyCount;
	Character * character = new Character(charid, type);

	// check if curr hexbank count greater than char cost
	if (buy && getHexbank() < character->getCost())
			return NULL;


	// save king in kingChar and rest in army
	if (type==king) {
		if (kingChar != NULL) 
			return NULL;
		kingChar = character;	
		army.push_back(character);
	} else {
		if (isArmyMax())
			return NULL;
		army.push_back(character);	
		if (buy)
			decHexbank(character->getCost());
	}
	return character;
}

	// Returns Character* of ID id.
	Character * Player::getCharacter(int id) {
		vector<Character*>::iterator it;
		int charID;
		for (it = army.begin(); it != army.end(); ++it) {
			charID = (*it)->getId();
			if (charID == id) {
				return *it;
			}
		}
		return NULL;
/*		for (size_t i=0; i<army.size(); i++) {
			if(army.at(i)->getId() == id)
				return army[i];
		}
		return NULL;	*/
	}

	// Returns the index in the army to the Character of CharID id.
	int Player::getCharacterIndex(int id) {
		for (size_t i=0; i<army.size(); i++) {
			if(army[i]->getId() == id)
				return i;
		}
		return -1;
	}

	// Deletes a Character from army with CharID id. Returns true if
	// successful.
	bool Player::delArmyChar(int id) {
		int i = getCharacterIndex(id);
		if (i>-1) {
			army.erase(army.begin()+i);
			return true;
		}
		return false;
	}

	// Returns current size of army.
	int Player::getArmySize() {
		return army.size();
	}

	// Returns max size of army.
	int Player::getMaxArmy() {
		return maxArmy;
	}

	// Returns true if current army is max size.
	bool Player::isArmyMax() {
		return getArmySize()==maxArmy;
	}

	// Returns the army vector
	vector<Character*> Player::getArmy() {
		return army;
	}

	// Returns index of x-coordinate tile of spawn point.
	int Player::getSpawnX() {
		switch (getIndex()) {
		case 1:
		case 3:
			return 0;
			break;
		case 2:
		case 4:
		default:
			return 9;
			break;
		}
	}

	// Returns index of y-coordinate tile of spawn point.
	int Player::getSpawnY() {
		switch (getIndex()) {
		case 1:
		case 4:
			return 0;
			break;
		case 2:
		case 3:
		default:
			return 9;
			break;
		}
	}

	// Updates army characters' cooldown timers.
	void Player::minusCooldown() {
		for (size_t i=0; i<army.size(); i++)
			army[i]->minusCooldown();
	}

	// Increases hexbank by amount n.
	void Player::incHexbank(int n) {
		hexbank += n;
	}

	// Decreases hexbank by amount n.
	void Player::decHexbank(int n) {
		hexbank -= n;
	}

	// Get current value of hexbank.
	int Player::getHexbank() {
		return hexbank;
	}

	// Returns false if king is still alive.
	bool Player::isDefeated() {
		return kingChar==NULL || kingChar->isDead();
	}

	// Sets whether player is still in the game
	void Player::isDefeated(bool b) {
		if (b)
			kingChar->kill();
	}

	// Returns true if player is ready to start game
	bool Player::isReady() {
		return ready;
	}

	// Sets whether player is ready to start game
	void Player::isReady(bool b) {
		ready = b;
	}