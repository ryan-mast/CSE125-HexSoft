#ifndef CHARACTER_H
#define CHARACTER_H 
#define LOCATION_UPDATE_INTERVAL 1500		// 1.5 seconds between location updates

#include <iostream>
#include <time.h>
#include "TinyXML/tinyxml.h"
//#include "../graphics/SG.h"
extern const char * typeStrings[];

enum Type { archer, healer, king, paladin, soldier, null };

/* for use with graphics only */
struct graphicsCoord
{
	float	xMove;
	float	yMove;
	float	zMove;

};

struct coord
{
	int x;
	int y;
	coord() {
		x = -1;
		y = -1;
	}
	coord(int x, int y) {
		x = x;
		y = y;
	}
public:
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
	void setX(int c) {
		x = c;
	}
	void setY(int c) {
		y = c;
	}
};

class Character {
	
	int id;
	Type type;
	int cost;
	int currHealth;
	int maxHealth;
	int attackDamage;
	int attackRange;
	int movementRange;
	int cooldown;
	int currCooldown;
	coord currLocation;
	coord destLocation;
	clock_t startTime;
	clock_t lastMoveTime;

	void initChar();	
	void initializeAttributes(Type type);
public:
	//MatrixTransform * node;
	//graphicsCoord gCoord;
	Character();
	Character::Character(int id, Type type);
	Character::~Character();

	// location coordinates
	bool isInitialized();
	coord getCurrCoord();
	void setCurrCoord(int x, int y);
	coord getDestCoord();
	void setDestCoord(int x, int y);
	bool updateLocation(Character *** board);

	// attributes
	int getId();
	int getParent();
	int getFullId();
	Type getType();
	int getCost();
	int getHealth();	
	int getMaxHealth();
	int getAttackDamage();
	int getAttackRange();
	int getMovementRange();

	// cooldown info
	void minusCooldown();
	int getCooldown();
	void resetCooldown();
	bool canMove();

	// game functions
	bool kill();
	bool isDead();
};

#endif