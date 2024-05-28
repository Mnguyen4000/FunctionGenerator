/*
 * g8_brightness.c
 *
 * Created: 28/03/2024 12:31:14 PM
 *  Author: marcu
 */

#include "g8_brightness.h"
#include "g8_EEPROM.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LCDPORT PORTC
#define LCDPIN 3
#define LCDDDR DDRC

volatile static int brightnessValue;

// Initialises backlight and pin interrupts.
void backlight_init(void)
{
	// Initialise the backlight level
	brightnessValue = read_brightness(); // Gets value from EEPROM

	LCDDDR |= (1 << LCDPIN);
}

// Make a function to initialise the buttons and set them to do brightness levels
void brightness(int percentage)
{
	// Checking Limits
	if (percentage > 100)
		percentage = 100;
	else if (percentage < 0)
		percentage = 0;

	DDRD |= (1 << 3);
	// PD6 is now an output
	int duty_cycle = (256 * percentage) / 100;

	if (percentage >= 100)
		duty_cycle = 255;
	else if (duty_cycle <= 0)
		duty_cycle = 0;

	OCR2B = duty_cycle;
	// set PWM for 50% duty cycle

	TCCR2A |= (1 << COM2B1);
	// set none-inverting mode

	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	// set fast PWM Mode

	TCCR2B |= (1 << CS22);
	// set prescaler to 8 and starts PWM
}

// Adjusts the brightness with the levels 1 to 5
// Level 1 being no light
// Level 5 being max light
void brightness_level(int level)
{
	// Checking Limits
	if (level > 5)
		level = 5;
	else if (level < 1)
		level = 1;

	switch (level) {
	case 1:
		// Turn off the backlight
		brightness(0);
		// Turn off the GPIO of the backlight supply voltage.
		LCDPORT &= ~(1 << LCDPIN);
		store_brightness(1);
		break;

	case 2:
		brightness(25);
		// Keep the GPIO of the backlight supply voltage ON.
		LCDPORT |= (1 << LCDPIN);
		store_brightness(2);
		break;

	case 3:
		brightness(50);
		// Keep the GPIO of the backlight supply voltage ON.
		LCDPORT |= (1 << LCDPIN);
		store_brightness(3);
		break;

	case 4:
		brightness(75);
		// Keep the GPIO of the backlight supply voltage ON.
		LCDPORT |= (1 << LCDPIN);
		store_brightness(4);
		break;

	case 5:
		brightness(100);
		// Keep the GPIO of the backlight supply voltage ON.
		LCDPORT |= (1 << LCDPIN);
		store_brightness(5);
		break;
		
	default:
		break;
	}
}

// Changes the backlight level according to the current brightness.
void update_backlight(void)
{
	brightness_level(read_brightness() - '0');
}
