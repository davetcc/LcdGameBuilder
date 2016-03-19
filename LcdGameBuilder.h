// LcdGameBuilder.h

#ifndef _LCDGAMEBUILDER_h
#define _LCDGAMEBUILDER_h

#include "arduino.h"
#include <LiquidCrystal.h>

#define SCREEN_DIVISOR 8

typedef unsigned char _board_pos;

struct GameConfig {
	char upPin;
	char downPin;
	char leftPin;
	char rightPin;
	char firePin;
	char boardSizeX;
	char boardSizeY;
};

class BoardComponent {
protected:
	GameConfig* gameConfig;
	_board_pos xPosition = 0;
	_board_pos yPosition = 0;
	const char* text = ".";

public:
	// get the actual X position in the game - not screen coordinates.
	_board_pos getXPosition() { return xPosition; }

	// get the actual Y position in the game - not screen coordinates.
	_board_pos getYPosition() { return yPosition; }

	// get the actual screen X position
	char getBoardX() { return xPosition / SCREEN_DIVISOR; }

	// get the actual screen Y position
	char getBoardY() { return yPosition / SCREEN_DIVISOR; }

	// manually move the component to a new X and Y location outside of game loop
	void moveTo(char x, char y) { xPosition = x * SCREEN_DIVISOR; yPosition = y * SCREEN_DIVISOR; }

	// get the text to display for this component.
	const char* getBoardPiece() { return text; }

	// has this component collided with 'other'
	bool hasCollidedWith(BoardComponent* other);

	// sets the game configuration so the board component can access dimensions
	void setGameConfig(GameConfig* config) { gameConfig = config; }
};

class Player : public BoardComponent {
public:
	void moveLeft()   { if (xPosition > 0) xPosition-=SCREEN_DIVISOR; }
	void moveRight()  { if (xPosition < gameConfig->boardSizeX * SCREEN_DIVISOR) xPosition+=SCREEN_DIVISOR; }
	void moveUp()     { if (yPosition > 0) yPosition-=SCREEN_DIVISOR; }
	void moveDown()   { if (yPosition < gameConfig->boardSizeY * SCREEN_DIVISOR) yPosition+=SCREEN_DIVISOR; }

	void initialise(const char* text, char x, char y);
};

// keyboard key assignments returned by checkKeyboard. Use bitwise and to check for 
// the bits that are set eg: ((keysPressed & KEYBOARD_??) != 0)
#define KEYBOARD_UP    0b00001
#define KEYBOARD_DOWN  0b00010
#define KEYBOARD_LEFT  0b00100
#define KEYBOARD_RIGHT 0b01000
#define KEYBOARD_FIRE  0b10000

// the baddie generally moves to the left
#define BADDIE_MOVES_LEFT      0x01
// the baddie generally moves to the right
#define BADDIE_MOVES_RIGHT     0x02
// the baddie generally moves up
#define BADDIE_MOVES_UP        0x04
// the baddie generally moves down
#define BADDIE_MOVES_DOWN      0x08
// the baddie tracks the player
#define BADDIE_TRACKS_PLAYER   0x10
// the baddie can be shot
#define BADDIE_CAN_BE_SHOT     0x20
// the baddie is controlled in the game loop
#define BADDIE_MOVED_BY_ME     0x40

class Baddie : public BoardComponent {
private:
	char baddieMovement;
	char speed;
	bool offBoard;
public:
	Baddie();
	// automatically move the baddie to a new position
	void move(Player *player);

	// is the baddie off the board at the moment
	bool isOffBoard() { return offBoard; }

	// reset the baddie completely to new parameters
	void reposition(const char *text, char baddieMovement, char x,char y, char speed);

	// set the baddie to be off the board
	void setOffBoard() { offBoard = true; }

	// check if the baddie can be shot
	bool canBeShot() { return (baddieMovement & BADDIE_CAN_BE_SHOT) != 0; }
private:
	void trackPlayerX(Player *player);
	void trackPlayerY(Player *player);
};

class Bullet : public BoardComponent {
private:
	bool inUse;
	char movement;
public:
	Bullet();
	void reposition(BoardComponent* component, char baddieMovement, char* text);
	bool isInUse() { return inUse; }
	void setCollided() { inUse = false; }
	void move();
};

class Explosion {
private:
	char x;
	char y;
	int counter;
public:
	Explosion() { counter = 0; x = y = 0; }
	void positionExplosion(BoardComponent *compoennt);
	void decreaseCounter() { counter--; }
	bool isOnBoard() { return counter > 0; }
	int getBoardX() { return x; }
	int getBoardY() { return y; }
};

class GameEngine {
private:
	char keysPressed;
	char debounceCount;

	Player player;
	Baddie *baddies;
	char numBaddies;
	Bullet playersBullet;
	GameConfig *config;
	LiquidCrystal *display;
	Explosion explosion;
public:
	GameEngine(LiquidCrystal *display, GameConfig &config, Baddie baddies[], char numBaddies);

	void initialisePlayer(const char* playerText, int xPosition, int yPosition);

	void awaitFire();

	void gameLoop();

	Bullet& getPlayerBullet() { return playersBullet; }
	Explosion& getExplosion() { return explosion; }
	Player& getPlayer() { return player; }

	int checkKeyboard();
	int playerCollisionCheck();
	int baddieCollisionCheck();
	void playerBulletReposition(char movement, char* bulletText);
	void render();
private:
	void checkKey(char pin, char bit);
	void inline assignKey(char pin, char bit);
};

#endif
