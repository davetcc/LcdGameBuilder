
#include <LiquidCrystal.h>
#include <LcdGameBuilder.h>

#define DISPLAY_CONTRAST_PIN 2
#define GAME_DELAY 30
#define ITEMS_TO_CATCH 3
#define BOARDSIZE_X 20
#define BOARDSIZE_Y 4

// we rely on the LiquidCrystal library. It must be initialised first.
LiquidCrystal lcd(52, 53, 46, 47, 48, 49);

void setup()
{
	// before anything else we need to initialise the lcd library
	lcd.begin(BOARDSIZE_X, BOARDSIZE_Y);

	// here we set the display contrast using PWM, use the keyboardTest sketch
	// to set up your keyboard and workout the brightness level first.
	pinMode(DISPLAY_CONTRAST_PIN, OUTPUT);
	analogWrite(DISPLAY_CONTRAST_PIN, 90);
}

void loop()
{
	instructions();
	
	// here's the baddies we'll have to catch
	Baddie baddies[ITEMS_TO_CATCH];

	// now we create a game configuration that tells our game engine
	// which inputs the keys are mapped to
	GameConfig config;
	config.downPin = 25;
	config.upPin = 24;
	config.leftPin = 22;
	config.rightPin = 23;
	config.firePin = 26;
	config.boardSizeX = BOARDSIZE_X;
	config.boardSizeY = BOARDSIZE_Y;

	// we need to create a game engine, this manages the keyboard and
	// all the components on the board.
	GameEngine game(&lcd, config, baddies, ITEMS_TO_CATCH);
	game.initialisePlayer("\_/", 8, 3);

	// wait until someone is ready to play
	game.awaitFire();

	// start off with 30 seconds to go and nothing saved from falling
	int itemsSaved = 0;
	int timeLeft = 1000;
	while (timeLeft > 0) {

		// the game runs too fast unless it is appropriately delayed.
		delay(GAME_DELAY);
		timeLeft--;

		// the game loop is where all the things on the board are moved
		// to new positions and the screen is redrawn
		game.gameLoop();

		// check if the player has "caught" any baddies.
		int item = game.playerCollisionCheck();
		if (item != -1) {
			// we have, reset the Baddie to a new position
			baddies[item].reposition("(*)", BADDIE_MOVES_DOWN, randomX(), 0, movementSpeed(timeLeft));
			itemsSaved++;
		}
			
		// otherwise have any baddies gone off the board because they were 
		// not caught.
		for (int i = 0; i < ITEMS_TO_CATCH; i++) {
			if(baddies[i].isOffBoard()) {
				baddies[i].reposition("(*)", BADDIE_MOVES_DOWN, randomX(), 0, movementSpeed(timeLeft));
			}
		}

		// finally render the number of seconds left.
		lcd.setCursor(0, 17);
		lcd.print("  ");
		lcd.setCursor(0, 17);
		lcd.print(timeLeft / GAME_DELAY);
	}

	// we are done, show the game over page and wait for the fire button
	gameOver(itemsSaved);
	game.awaitFire();
}

/* this produces a random position for the next baddie repositioning */
char randomX() {
	return (random() % (BOARDSIZE_X - 3));
}

/* this produces a random speed for the next baddie repositioning */
char movementSpeed(int time) {
	int adjust = 10 - (time / 100);
	return (random() % adjust) + 1;
}

void instructions() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("CatchEm! Let nothing");
	lcd.setCursor(0, 1);
	lcd.print("get past your ship..");
	lcd.setCursor(0, 3);
	lcd.print("thecoderscorner.com");
}

void gameOver(int itemsSaved) {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Out of time");
	lcd.setCursor(0, 1);
	lcd.print("Items saved");
	lcd.setCursor(15, 1);
	lcd.print(itemsSaved);
	lcd.setCursor(0, 2);
	lcd.print("Fire to try again");
	lcd.setCursor(0, 3);
	lcd.print("thecoderscorner.com");
}
