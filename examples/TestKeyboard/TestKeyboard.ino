#include <LcdGameBuilder.h>
#include <LiquidCrystal.h>

/**
  Distributed by thecoderscorner.com, under an apache license

  This simple application written to the LcdGameBuilder tests that the
  keyboard is configured correctly by representing each key state on
  the display as on or off. You may find it easier to get the screen
  and keyboard working with this before trying an all out game.
  This program also allows you to get the contrast right (which this
  library controls via PWM analog output). Adjust the contrast by
  pressing the up and down keys.
*/

LiquidCrystal lcd(52, 53, 46, 47, 48, 49);
int displayBrightness = 80;
void setup()
{
	lcd.begin(20, 4);
	pinMode(2, OUTPUT);
}

void loop()
{
	// we need to set up baddies, but they can be empty for this
	Baddie baddies[1];

	// configure the keyboard pins. they will be configured as input
	// by the library.
	GameConfig config;
	config.downPin = 25;
	config.upPin = 24;
	config.leftPin = 22;
	config.rightPin = 23;
	config.firePin = 26;

	// create a game engine, it will do keyboard input for us.
	GameEngine engine(&lcd, config, baddies, 0);


	while (true) {
		analogWrite(2, displayBrightness);

		// slow things down a bit.. actually needed for gaming
		delay(50);

		// get the state of the keys as a bitmask.
		int keys = engine.checkKeyboard();

		// Output Up button state.
		lcd.setCursor(0, 0);
		lcd.print("Up");
		lcd.setCursor(10, 0);
		lcd.print((keys & KEYBOARD_UP) ? "YES" : "NO ");

		// Output down button state
		lcd.setCursor(0, 1);
		lcd.print("Down");
		lcd.setCursor(10, 1);
		lcd.print((keys & KEYBOARD_DOWN) ? "YES" : "NO ");

		// Output left button state
		lcd.setCursor(0, 2);
		lcd.print("Left");
		lcd.setCursor(10, 2);
		lcd.print((keys & KEYBOARD_LEFT) ? "YES" : "NO ");

		// Output right button state
		lcd.setCursor(0, 3);
		lcd.print("Right");
		lcd.setCursor(10, 3);
		lcd.print((keys & KEYBOARD_RIGHT) ? "YES" : "NO ");

		// Lastly output if fire is pressed.
		lcd.setCursor(15, 3);
		lcd.print((keys & KEYBOARD_FIRE) ? "Fire" : "    ");

		// check if the up or down have been pressed and adjust Vo as appropriate
		if ((keys&KEYBOARD_UP) && displayBrightness < 255) {
			displayBrightness++;
		}
		else if ((keys&KEYBOARD_DOWN) && displayBrightness > 0) {
			displayBrightness--;
		}

		// now we display the contrast of the display (the Vo pin setting)
		// you can find the perfect setting for your display, then the use 
		// that setting in all the games.
		lcd.setCursor(15, 0);
		lcd.print("Cont");
		lcd.setCursor(15, 1);
		lcd.print("    ");
		lcd.setCursor(15, 1);
		lcd.print(displayBrightness);
	}

}
