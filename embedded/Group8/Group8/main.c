/*
 * Group8.c
 *
 * Created: 27/02/2024 11:10:37 PM
 * Author : marcu
 */
#include "u8g2/u8g2.h"
#include "u8x8_avr.h"
#include "g8_i2c.h"
#include "uart.h"
#include "g8_brightness.h"
#include "g8_EEPROM.h"
#include "g8_button.h"
#include "g8_DAC.h"
#include "g8_pot.h"

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

u8g2_t u8g2;

#define CS_DDR DDRB
#define CS_PORT PORTB
#define CS_BIT 2

#define DC_DDR DDRB
#define DC_PORT PORTB
#define DC_BIT 1

#define RESET_DDR DDRB
#define RESET_PORT PORTB
#define RESET_BIT 0

struct WaveConfig data;

u8g2_t u8g2;
uint8_t
u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	// Re-use library for delays
	if (u8x8_avr_delay(u8x8, msg, arg_int, arg_ptr))
		return 1;

	switch (msg) {
	// called once during init phase of u8g2/u8x8
	// can be used to setup pins
	case U8X8_MSG_GPIO_AND_DELAY_INIT:
		CS_DDR |= _BV(CS_BIT);
		DC_DDR |= _BV(DC_BIT);
		RESET_DDR |= _BV(RESET_BIT);
		break;
	// CS (chip select) pin: Output level in arg_int
	case U8X8_MSG_GPIO_CS:
		if (arg_int)
			CS_PORT |= _BV(CS_BIT);
		else
			CS_PORT &= ~_BV(CS_BIT);
		break;
	// DC (data/cmd, A0, register select) pin: Output level in arg_int
	case U8X8_MSG_GPIO_DC:
		if (arg_int)
			DC_PORT |= _BV(DC_BIT);
		else
			DC_PORT &= ~_BV(DC_BIT);
		break;
	// Reset pin: Output level in arg_int
	case U8X8_MSG_GPIO_RESET:
		if (arg_int)
			RESET_PORT |= _BV(RESET_BIT);
		else
			RESET_PORT &= ~_BV(RESET_BIT);
		break;
	default:
		u8x8_SetGPIOResult(u8x8, 1);
		break;
	}
	return 1;
}

void update_screen(struct WaveConfig data, bool is_gui_connected)
{
	u8g2_ClearBuffer(&u8g2);

	int circle_x = 120;
	int circle_y = 15;
	int circle_radius = 5;
	if (is_gui_connected)
		u8g2_DrawDisc(&u8g2, circle_x, circle_y, circle_radius, U8G2_DRAW_ALL);
	else
		u8g2_DrawCircle(&u8g2, circle_x, circle_y, circle_radius, U8G2_DRAW_ALL);

	// WAVE TYPE
	if (get_selected() == 0) {
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
		u8g2_DrawStr(&u8g2, 0, 10, "Wave Type ");
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
	} else {
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(&u8g2, 0, 10, "Wave Type ");
	}
	switch (atoi(data.Type)) {
	case 1:
		u8g2_DrawStr(&u8g2, 65, 10, "DC");
		break;
	case 2:
		u8g2_DrawStr(&u8g2, 65, 10, "Sine");
		break;
	case 3:
		u8g2_DrawStr(&u8g2, 60, 10, "Square");
		break;
	case 4:
		u8g2_DrawStr(&u8g2, 65, 10, "Triangle");
		break;
	case 5:
		u8g2_DrawStr(&u8g2, 60, 10, "Sawtooth");
		break;
	case 6:
		u8g2_DrawStr(&u8g2, 60, 10, "ReSawtooth");
		break;
	default:
		u8g2_DrawStr(&u8g2, 60, 10, "Undefined");
		break;
	}

	// AMPLITUDE
	if (get_selected() == 1) {
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
		u8g2_DrawStr(&u8g2, 0, 30, "Amplitude ");
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
	} else {
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(&u8g2, 0, 30, "Amplitude ");
	}

	u8g2_DrawStr(&u8g2, 55, 30, data.Ampl);

	// FREQUENCY
	if (get_selected() == 2) {
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
		u8g2_DrawStr(&u8g2, 0, 40, "Frequency ");
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
	} else {
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(&u8g2, 0, 40, "Frequency ");
	}

	char freq[10];
	strcpy(freq, data.Freq);
	for (int i = 0; i < 5; i++) {
		if (freq[0] == '0') {
			freq[0] = freq[1];
			freq[1] = freq[2];
			freq[2] = freq[3];
			freq[3] = freq[4];
			freq[4] = '\0';
		} else {
			break;
		}
	}
	u8g2_DrawStr(&u8g2, 55, 40, freq);

	// OFFSET
	if (get_selected() == 3) {
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
		u8g2_DrawStr(&u8g2, 0, 50, "Offset ");
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
	} else {
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(&u8g2, 0, 50, "Offset ");
	}
	u8g2_DrawStr(&u8g2, 55, 50, data.Offset);

	// BRIGHTNESS
	if (get_selected() == 4) {
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
		u8g2_DrawStr(&u8g2, 0, 60, "Brightness ");
		u8g2_SetFont(&u8g2, u8g2_font_6x13_tf);
	} else {
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(&u8g2, 0, 60, "Brightness ");
	}
	char bright[20];
	sprintf(bright, "%c", read_brightness());
	u8g2_DrawStr(&u8g2, 65, 60, bright);

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	float frequency = atof(data.Freq);
	float period;
	if (frequency > 1000) {
		period = (1.0 / frequency) * 1000000.0;
	} else {
		period = (1.0 / frequency) * 1000.0;
	}
	int integerPart = (int)period;
	float decimalPart = period - integerPart;
	char periodStr[20];
	if (frequency > 1000) {
		snprintf(periodStr, sizeof(periodStr), "%d.%dus", integerPart, (int)(decimalPart * 10));
	} else {
		snprintf(periodStr, sizeof(periodStr), "%d.%dms", integerPart, (int)(decimalPart * 10));
	}
	u8g2_DrawStr(&u8g2, 90, 30, "Period: ");
	u8g2_DrawStr(&u8g2, 90, 40, periodStr);

	float amplitude = atof(data.Ampl);
	float peakToPeak = amplitude * 2;
	integerPart = (int)peakToPeak;
	decimalPart = peakToPeak - integerPart;
	int displayDecimal = (int)((decimalPart * 10) + 0.5);
	char peakToPeakStr[20];
	snprintf(peakToPeakStr, sizeof(peakToPeakStr), "%d.%d", integerPart, displayDecimal);
	u8g2_DrawStr(&u8g2, 0, 20, "Peak to Peak: ");
	u8g2_DrawStr(&u8g2, 65, 20, peakToPeakStr);

	if (clip_check(data)) {
		u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
		u8g2_DrawGlyph(&u8g2, 100, 60, 0x2614);
	}

	u8g2_SendBuffer(&u8g2);
}

// compare the date stored and the current data
int is_wave_config_same(struct WaveConfig *a, struct WaveConfig *b)
{
	if (strcmp(a->Type, b->Type) != 0)
		return 0;
	if (strcmp(a->Freq, b->Freq) != 0)
		return 0;
	if (strcmp(a->Ampl, b->Ampl) != 0)
		return 0;
	if (strcmp(a->Offset, b->Offset) != 0)
		return 0;
	return 1;
}

// Main Loop
int main(void)
{
	// Checks if the EEPROM is initialised.
	eeprom_validity();

	// Initialise the modules
	uart_init(UART_BAUD_SELECT(19200, F_CPU));
	spi_init();
	i2c_init();
	backlight_init();
	init_buttons();
	sei();

	// Must be initialised in main()
	u8g2_Setup_st7920_s_128x64_f(
		&u8g2, U8G2_R0,
		u8x8_byte_avr_hw_spi,
		u8x8_gpio_and_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);

	bool is_gui_connected = false;
	struct WaveConfig last_sent_data;
	memset(&last_sent_data, 0, sizeof(struct WaveConfig));
	int timeout_count = 0;
	read_data(&data);
	send_data_to_gui();

	DDRC |= (1 << 1);
	
	int count = 0;
	while (1) {
		// Reads the uart buffer string.
		char buffer[30];

		for (int i = 0; i < 30; i++) {
			buffer[i] = uart_getc();
			if (buffer[i] == 'c') {
				is_gui_connected = true;
				memset(buffer, 0, sizeof(buffer));
				send_data_to_gui();
				timeout_count = 0;
				i = -1; // Reset the index, the next loop will start at buffer[0]
				continue;
			}

			if (buffer[i] == '\n') {
				store_data(buffer); // Store Data into EEPROM
				break;
			}
		}

		timeout_count++;
		if (timeout_count > 8) {
			is_gui_connected = false;
			timeout_count = 0;
		}
		// send string to gui only when store_data changed
		if (!is_wave_config_same(&data, &last_sent_data)) {
			send_data_to_gui();
			last_sent_data = data;
		}

		// Perform any action to do with the buttons.
		button_operations(&data);

		// Reads current EEPROM data
		read_data(&data);

		// Updates LCD Screen
		update_screen(data, is_gui_connected);

		// Updates Backlight brightness
		update_backlight();

		if (read_state() == '1')
			PORTC |= (1 << 1);
		else
			PORTC &= ~(1 << 1);
		
		count += 100;
		if (count >= 4096) {
			count = 0;
		}
		int offset = ((atof(data.Offset) + 5) / 10 * 4096);
		voltage_out(4096 - offset);
		_delay_ms(15);
	}
	return 0;
}