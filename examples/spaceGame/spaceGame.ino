
#define SCREEN_X 20
#define SCREEN_Y 4

#define NO_OF_BADDIES 4
#define ASTEROID_BADDIES 2

#include<string.h>
#include <LiquidCrystal.h>
#include "LcdGameBuilder.h"

LiquidCrystal lcd(52, 53, 46,47, 48, 49);  

/**
  SpaceGame - a platform game for LiquidCrystal library by thecoderscorner.com. Creative commons sharealike.

  This game for the game builder library demos most of what is possible with the library. It is a 
  platform game where the goal is to fly through as many levels as possible.
*/
void setup()  {
    lcd.begin(SCREEN_X, SCREEN_Y);

	pinMode(2, OUTPUT);
	analogWrite(2, 90);
}

void loop()  {
	// as with all games, they start off on the instructions screen.
	instructions();


	Baddie baddies[NO_OF_BADDIES];

	GameConfig config;
	config.downPin = 25;
	config.upPin = 24;
	config.leftPin = 22;
	config.rightPin = 23;
	config.firePin = 26;
	config.boardSizeX = SCREEN_X;
	config.boardSizeY = SCREEN_Y;

	// now we create the game engine and wait for fire to be pressed.
	GameEngine game(&lcd, config, baddies, NO_OF_BADDIES);
	game.initialisePlayer("->", 1, 2);
	game.awaitFire();

	float score = 0.0;

	// while we've not hit a baddie
	while (game.playerCollisionCheck() == -1) {
		delay(delayFromScore(score));
		game.gameLoop();

		if (game.checkKeyboard() & KEYBOARD_FIRE) {
			game.playerBulletReposition(BADDIE_MOVES_RIGHT, "-");
		}

		for (int i = 0; i < ASTEROID_BADDIES; i++) {
			if (baddies[i].isOffBoard()) {
				baddies[i].reposition("*", BADDIE_MOVES_LEFT, 17, randomY(), 2 * i);
			}
		}	

		for (int i = ASTEROID_BADDIES; i < NO_OF_BADDIES; i++) {
			if (baddies[i].isOffBoard()) {
				baddies[i].reposition("<o>", BADDIE_MOVES_LEFT | BADDIE_TRACKS_PLAYER | BADDIE_CAN_BE_SHOT, 17, randomY(), 3 * i);
			}
		}
		score = score + 0.1;

		lcd.setCursor(14, 0);
		lcd.print((int)score);

		int collisionBaddie = game.baddieCollisionCheck();
		if (collisionBaddie != -1) {
			game.getExplosion().positionExplosion(&baddies[collisionBaddie]);
			baddies[collisionBaddie].setOffBoard();
			game.getPlayerBullet().setCollided();
			score = score + 10;
		}
	}

	// and then we show the game over page and wait for fire
	gameOver(score);
	game.awaitFire();
}

void instructions() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Play space fighter");
    lcd.setCursor(0,1);
    lcd.print("Press fire to start");
    lcd.setCursor(0,3);
    lcd.print("thecoderscorner.com");
}

void gameOver(int score) {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Game Over");
	lcd.setCursor(0, 1);
	lcd.print("Score");
	lcd.setCursor(10, 1);
	lcd.print(score);
	lcd.setCursor(0, 2);
	lcd.print("Fire to try again");
	lcd.setCursor(0, 3);
	lcd.print("thecoderscorner.com");
}

int delayFromScore(int score) {
	return max(15, 50 - (score / 10));
}

char randomY() {
	return (random() % SCREEN_Y);
}
