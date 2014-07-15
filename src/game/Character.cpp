#include "Character.h"

using namespace std;

const char * typeStrings[] = {"archer", "healer", "king", "paladin", "soldier", " "};

Character::Character() {
	this->type = null;
	cost = currHealth = maxHealth = -1;
	attackDamage = attackRange = movementRange = -1;
	initChar();
}

Character::Character(int id, Type type) {
	this->id = id;
	this->type = type;
	initializeAttributes(type);
	initChar();
	
}

Character::~Character() {
}

// Initialize class variables.
void Character::initChar() {	
	cooldown = 5;
	currCooldown = 0;
	lastMoveTime = clock();
	currLocation = coord();
	destLocation = coord();
	startTime = clock();
}

// Initializes class values with info from attributes.xml
void Character::initializeAttributes(Type type)
{
	TiXmlDocument doc("attributes.xml");
	TiXmlNode *node = 0;

	if (doc.LoadFile()) {
		node = doc.RootElement();
		assert(node);
		node = node->FirstChildElement(typeStrings[type]);	// gets the node for the type of character
		assert(node);

		cost = atoi(node->FirstChild("cost")->FirstChild()->Value());
		currHealth = maxHealth = atoi(node->FirstChild("health")->FirstChild()->Value());
		attackDamage = atoi(node->FirstChild("attackDamage")->FirstChild()->Value());
		attackRange = atoi(node->FirstChild("attackRange")->FirstChild()->Value());
		movementRange = atoi(node->FirstChild("movementRange")->FirstChild()->Value());
	}
	else {
		cout << "load failed\n";
	}
}

// Returns true if Character has been placed on board.
bool Character::isInitialized() {
	return currLocation.getX()>-1 && currLocation.getY()>-1;
}

// Returns a coord of current coordinates.
coord Character::getCurrCoord(){
	return currLocation;
}

// Sets current coordinations to (x,y).
void Character::setCurrCoord(int x, int y){
	currLocation.setX(x);
	currLocation.setY(y);
}

// Returns destination coordinations
coord Character::getDestCoord(){
	return destLocation;
}

// Sets destionation coordinations to (x,y).
void Character::setDestCoord(int x, int y){
	destLocation.setX(x);
	destLocation.setY(y);
}

bool Character::updateLocation(Character *** board) {

	int currX = currLocation.getX();
	int currY = currLocation.getY();
	int destX = destLocation.getX();
	int destY = destLocation.getY();

	// too soon, just return
	if ((clock()-lastMoveTime) < LOCATION_UPDATE_INTERVAL) {
		return false;
	}

	if (destX == -1 || destY == -1) {		// destination not set, just return
		return false;
	} else if ( currX == destX && currY == destY) {	// currently at destination, just return
		return false;
	} else if (currX < destX && board[currX+1][currY] == NULL) {
		setCurrCoord(currX+1, currY);
		board[currX+1][currY] = this;
	} else if (currX > destX && board[currX-1][currY] == NULL) {
		setCurrCoord(currX-1, currY);
		board[currX-1][currY] = this;
	} else if (currY < destY && board[currX][currY+1] == NULL) {
		setCurrCoord(currX, currY+1);
		board[currX][currY+1] = this;
	} else if (currY > destY && board[currX][currY-1] == NULL) {
		setCurrCoord(currX, currY-1);
		board[currX-1][currY-1] = this;
	}
	board[currX][currY] = NULL;
	lastMoveTime = clock();
	return true;
}

// Returns CharID.
int Character::getId()
{
	return id%1000;
}

// Returns parent PlayerID.
int Character::getParent()
{
	return id/1000;
}

// Returns full CharID with parent.
int Character::getFullId()
{
	return id;
}

// Returns character type.
Type Character::getType()
{
	return type;
}

// Returns cost of character from store.
int Character::getCost()
{
	return cost;
}

// Returns character's current health.
int Character::getHealth()
{
	return currHealth;
}

// Returns character's max health.
int Character::getMaxHealth()
{
	return maxHealth;
}

// Returns character's attack damage.
int Character::getAttackDamage()
{
	return attackDamage;
}

// Returns character's attack range.
int Character::getAttackRange()
{
	return attackRange;
}

// Returns character's movement range.
int Character::getMovementRange()
{
	return movementRange;
}

// Updates character's cooldown timer.
void Character::minusCooldown() {
	if (currCooldown != 0)
		currCooldown = cooldown - (int)((clock()-startTime)/CLOCKS_PER_SEC);
	if (currCooldown < 0)
		currCooldown = 0;
}

// Returns character's current cooldown timer.
int Character::getCooldown() {
	return currCooldown;
}

// Resets character's cooldown timer.
void Character::resetCooldown() {
	currCooldown = cooldown;
	startTime = clock();
}

// Returns true if cooldown timer is at 0.
bool Character::canMove() {
	return currCooldown == 0;
}

// Returns true if character successfully killed
bool Character::kill() {
	if (currHealth!=0) {
		currHealth = 0;
		return true;
	}
	return false;
}

// Returns true if character is dead
bool Character::isDead() {
	if (currHealth>0)
		return false;
	return true;
}