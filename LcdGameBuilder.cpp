// 
// (C) 2018 thecoderscorner.com - under an apache license. 
// 

#include "LcdGameBuilder.h"

GameEngine::GameEngine(LiquidCrystal *display, GameConfig &gameConfig, Baddie *baddies, char numBaddies) {
	keysPressed = 0;
	this->baddies = baddies;
	this->config = &gameConfig;
	this->display = display;
	this->numBaddies = numBaddies;

	pinMode(config->downPin, INPUT);
	pinMode(config->upPin, INPUT);
	pinMode(config->leftPin, INPUT);
	pinMode(config->rightPin, INPUT);
	pinMode(config->firePin, INPUT);

	for (int i = 0; i < numBaddies; i++) {
		baddies[i].setGameConfig(&gameConfig);
	}
}

void GameEngine::initialisePlayer(const char* playerText, int xPosition, int yPosition) {
	player.initialise(playerText, xPosition, yPosition);
	player.setGameConfig(config);
}

void GameEngine::awaitFire() {
	while((keysPressed & 16) == 0) {
		delay(10);
		checkKeyboard();
	}

	while ((keysPressed & 16) != 0) {
		delay(10);
		checkKeyboard();
	}
}

char counter = 0;
void GameEngine::gameLoop() {

	checkKeyboard();

	if (playersBullet.isInUse()) {
		playersBullet.move();
	}

	if ((++counter % 3) == 0) {
		// player movement and collision detect
		if (keysPressed & KEYBOARD_UP) player.moveUp();
		if (keysPressed & KEYBOARD_DOWN) player.moveDown();
		if (keysPressed & KEYBOARD_LEFT) player.moveLeft();
		if (keysPressed & KEYBOARD_RIGHT) player.moveRight();
	}

	if ((++counter % 5) == 0) {
		for (char i = 0; i < numBaddies; i++) {
			baddies[i].move(&player);
		}
		render();
	}
}

int GameEngine::checkKeyboard() {
	if (debounceCount > 0) {
		if (--debounceCount == 0) {
			assignKey(config->upPin, KEYBOARD_UP);
			assignKey(config->downPin, KEYBOARD_DOWN);
			assignKey(config->leftPin, KEYBOARD_LEFT);
			assignKey(config->rightPin, KEYBOARD_RIGHT);
			assignKey(config->firePin, KEYBOARD_FIRE);
		}
	}
	else {
		checkKey(config->upPin, KEYBOARD_UP);
		checkKey(config->downPin, KEYBOARD_DOWN);
		checkKey(config->leftPin, KEYBOARD_LEFT);
		checkKey(config->rightPin, KEYBOARD_RIGHT);
		checkKey(config->firePin, KEYBOARD_FIRE);
	}
	return keysPressed;
}

void GameEngine::checkKey(char pin, char bit) {
	bool pressed = digitalRead(pin);
	if (pressed != ((keysPressed & bit) != false)) {
		debounceCount = 2;
	}
}

void GameEngine::assignKey(char pin, char bit) {
	keysPressed &= ~bit;
	if (digitalRead(pin)) {
		keysPressed |= bit;
	}
	display->setCursor(13, 3);
}

bool valInRange(int value, int min, int max) {
	return (value >= min) && (value <= max);
}

int GameEngine::playerCollisionCheck() {
	for (char i = 0; i < numBaddies; ++i) {
		if (!baddies[i].isOffBoard()) {
			if (baddies[i].hasCollidedWith(&player)) {
				return i;
			}
		}
	}
	return -1;
}

int GameEngine::baddieCollisionCheck() {
	for (char i = 0; i < numBaddies; ++i) {
		if (!baddies[i].isOffBoard() && baddies[i].canBeShot() && playersBullet.isInUse()) {
			if (baddies[i].hasCollidedWith(&playersBullet)) {
				return i;
			}
		}
	}
	return -1;
}

void GameEngine::playerBulletReposition(char movement, char* bulletText) {
	playersBullet.reposition(&player, movement, bulletText);
	playersBullet.setGameConfig(config);
}

void GameEngine::render(){
	display->clear();
	display->setCursor(player.getBoardX(), player.getBoardY());
	display->print(player.getBoardPiece());
	for (int i = 0; i < numBaddies; ++i) {
		if(!baddies[i].isOffBoard()) {
			display->setCursor(baddies[i].getBoardX(), baddies[i].getBoardY());
			display->print(baddies[i].getBoardPiece());
		}
	}

	if(playersBullet.isInUse()) {
		display->setCursor(playersBullet.getBoardX(), playersBullet.getBoardY());
		display->print(playersBullet.getBoardPiece());
	}

	if (explosion.isOnBoard()) {
		explosion.decreaseCounter();
		display->setCursor(explosion.getBoardX(), explosion.getBoardY());
		display->print("*88*");
	}
}

void Player::initialise(const char * text, char x, char y) {
	xPosition = x * SCREEN_DIVISOR;
	yPosition = y * SCREEN_DIVISOR;
	this->text = text;
}


// Baddie..

Baddie::Baddie() {
	offBoard = true;
}

void Baddie::move(Player* player) {
	if (isOffBoard() || (baddieMovement & BADDIE_MOVED_BY_ME)) return;

	if (baddieMovement & BADDIE_MOVES_DOWN) {
		yPosition+= speed;
		trackPlayerX(player);
	}
	else if (baddieMovement & BADDIE_MOVES_UP) {
		trackPlayerX(player);
		yPosition-= speed;
	}
	else if (baddieMovement & BADDIE_MOVES_LEFT) {
		trackPlayerY(player);
		xPosition-= speed;
	}
	else if (baddieMovement & BADDIE_MOVES_RIGHT) {
		trackPlayerY(player);
		xPosition+=speed;
	}
	else {
		trackPlayerX(player);
		trackPlayerY(player);
	}

	if (getBoardX() > gameConfig->boardSizeX || getBoardY() > gameConfig->boardSizeY || yPosition < 0 || xPosition < 0) {
		offBoard = true;
	}
}

void Baddie::reposition(const char* text, char movement, char x, char y, char speed) {
	xPosition = x * SCREEN_DIVISOR;
	yPosition = y * SCREEN_DIVISOR;
	this->text = text;
	this->baddieMovement = movement;
	this->speed = speed;

	offBoard = false;
}

void Baddie::trackPlayerX(Player *player) {
	if(baddieMovement & BADDIE_TRACKS_PLAYER) {
		if (xPosition < player->getXPosition() && xPosition < gameConfig->boardSizeX) xPosition++;
		if (xPosition > player->getXPosition() && xPosition > 0) xPosition--;
	}
}

void Baddie::trackPlayerY(Player *player) {
	if (baddieMovement & BADDIE_TRACKS_PLAYER) {
		if (yPosition < player->getYPosition()) yPosition++;
		if (yPosition > player->getYPosition()) yPosition--;
	}
}

Bullet::Bullet() {
	inUse = false;
}

void Bullet::reposition(BoardComponent* component, char baddieMovement, char* text) {
	movement = baddieMovement;
	if ((baddieMovement & BADDIE_MOVES_LEFT)!=0 || (baddieMovement & BADDIE_MOVES_RIGHT)!=0) {
		xPosition = component->getXPosition();
		if ((baddieMovement & BADDIE_MOVES_RIGHT) != 0) {
			xPosition = xPosition + (strlen(component->getBoardPiece())*SCREEN_DIVISOR);
		}
		yPosition = component->getYPosition();
	}
	else {
		xPosition = component->getXPosition();
		yPosition = component->getYPosition();
	}
	inUse = true;
}

void Bullet::move() {
	if (!inUse) return;

	if ((movement & BADDIE_MOVES_LEFT) != 0) xPosition-=SCREEN_DIVISOR;
	else if ((movement & BADDIE_MOVES_RIGHT) != 0) xPosition+=SCREEN_DIVISOR;
	else  if ((movement & BADDIE_MOVES_UP) != 0) yPosition-=SCREEN_DIVISOR;
	else if ((movement & BADDIE_MOVES_DOWN) != 0) yPosition+=SCREEN_DIVISOR;
	
	inUse = !(getBoardX() > gameConfig->boardSizeX || getBoardY() > gameConfig->boardSizeY || yPosition < 0 || xPosition < 0);
}

bool BoardComponent::hasCollidedWith(BoardComponent* other) {
	if (getBoardY() == other->getBoardY()) {
		int end = getBoardX() + strlen(getBoardPiece());
		int otherEnd = other->getBoardX() + strlen(other->getBoardPiece());
		if (valInRange(getBoardX(), other->getBoardX(), otherEnd) || valInRange(other->getBoardX(), getBoardX(), end)) {
			return true;
		}
	}
	return false;
}

void Explosion::positionExplosion(BoardComponent *compoennt) {
	counter = 2; 
	x = compoennt->getBoardX(); 
	y = compoennt->getBoardY(); 
}
