/*
 * g8_button.c
 *
 * Created: 19/04/2024 2:43:11 PM
 *  Author: marcu
 */

#include "g8_button.h"
#include "g8_EEPROM.h"

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TYPE 0
#define AMPLITUDE 1
#define FREQUENCY 2
#define OFFSET 3
#define BRIGHTNESS 4

void init_buttons(void)
{
	// Initialise the pushbuttons
	DDRB &= ~((1 << 6) || (1 << 7));
	DDRD &= ~(1 << 5);
}

int check_buttons(void)
{
	if (PINB & (1 << 6))
		return 6;
	else if (PINB & (1 << 7))
		return 7;
	else if (PIND & (1 << 5))
		return 5;
	else
		return 0;
}

static int prev_button = 0;
static int count = 0;
static int selected_parameter = TYPE; // Default selected parameter to wave type.

void button_operations(struct WaveConfig *data)
{
	if (check_buttons() == 6 && check_buttons() != prev_button) {
		// Decreases the value
		char buffer[10];
		int value;
		float valuef;
		switch (selected_parameter) {
		case TYPE:
			value = atoi(data->Type);
			value--;
			if (value < 1) // At the lowest limit, do nothing.
				value = 1;
			snprintf(buffer, 5, "%1i", value);
			store_type(buffer);
			break;

		case AMPLITUDE:
			valuef = atof(data->Ampl);
			value = valuef * 100;
			value++;
			value /= 5;
			value--;
			value *= 5;
			if (value < 100) // At the lowest limit, do nothing.
				value = 100;
			snprintf(buffer, 6, "%i.%02i", value / 100, value % 100);
			store_amplitude(buffer);
			break;

		case FREQUENCY:
			value = atoi(data->Freq);
			if (value > 1000)
				value -= 1000;
			else if (value <= 1000 && value > 100)
				value -= 100;
			else if (value <= 100 && value > 10)
				value -= 10;
			else if (value <= 10)
				value--;

			if (value < 1) // At the lowest limit, do nothing.
				value = 1;
			snprintf(buffer, 6, "%5i", value);
			store_frequency(buffer);
			break;

		case OFFSET:
			valuef = atof(data->Offset);
			value = valuef * 10;
			value--;
			if (value < -50) // At the lowest limit, do nothing.
				value = -50;
			if (value < 0) {
				value *= -1;
				snprintf(buffer, 6, "-%i.%01i", value / 10, value % 10);
			} else {
				snprintf(buffer, 6, "%i.%01i", value / 10, value % 10);
			}
			store_offset(buffer);
			break;

		case BRIGHTNESS:
			value = read_brightness() - '0';
			if (value <= 1) // At the lowest limit, do nothing.
				value = 1;
			else
				value--;
			snprintf(buffer, 5, "%1i", value);
			store_brightness(buffer[0]);
			break;
		}
		prev_button = check_buttons();
	} else if (check_buttons() == 7 && check_buttons() != prev_button) {
		// Switches the parameter
		if (selected_parameter == BRIGHTNESS)
			selected_parameter = TYPE;
		else
			selected_parameter++;
		prev_button = check_buttons();
	} else if (check_buttons() == 5 && check_buttons() != prev_button) {
		// Increases the value
		char buffer[10];
		int value;
		float valuef;
		switch (selected_parameter) {
		case TYPE:
			value = atoi(data->Type);
			value++;
			if (value > 6) // At the lowest limit, do nothing.
				value = 6;
			snprintf(buffer, 5, "%1i", value);
			store_type(buffer);
			break;

		case AMPLITUDE:
			valuef = atof(data->Ampl);
			value = valuef * 100;
			value++;
			value /= 5;
			value++;
			value *= 5;
			snprintf(buffer, 5, "%i.%02i", value / 100, value % 100);
			store_amplitude(buffer);
			break;

		case FREQUENCY:
			value = atoi(data->Freq);
			if (value >= 1000)
				value += 1000;
			else if (value < 1000 && value >= 100)
				value += 100;
			else if (value < 100 && value >= 10)
				value += 10;
			else if (value < 10)
				value++;

			if (value > 20000) // At the lowest limit, do nothing.
				value = 20000;
			snprintf(buffer, 6, "%5i", value);
			store_frequency(buffer);
			break;

		case OFFSET:
			valuef = atof(data->Offset);
			value = valuef * 10;
			value++;
			if (value > 50) // At the lowest limit, do nothing.
				value = 50;
				
			if (value < 0) {
				value *= -1;
				snprintf(buffer, 6, "-%i.%01i", value / 10, value % 10);
			} else {
				snprintf(buffer, 6, "%i.%01i", value / 10, value % 10);
			}
			store_offset(buffer);
			break;

		case BRIGHTNESS:
			value = read_brightness() - '0';
			if (value > 5) // At the lowest limit, do nothing.
				value = 5;
			else
				value++;
			snprintf(buffer, 5, "%1i", value);
			store_brightness(buffer[0]);
			break;

		}
		prev_button = check_buttons();
	} else if (check_buttons() == 0 && check_buttons() != prev_button) {
		// No button is being pressed...
		prev_button = check_buttons();
	}
}

int get_count(void)
{
	return count;
}

int get_selected(void)
{
	return selected_parameter;
}