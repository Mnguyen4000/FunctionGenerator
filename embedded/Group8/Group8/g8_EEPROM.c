/*
 * g8EEPROM.c
 *
 * Created: 28/03/2024 10:11:49 AM
 *  Author: marcu
 */

#include "g8_EEPROM.h"
#include <avr/eeprom.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h.>
#include "uart.h"

// Writes to the EEPROM
void store_data(char *data)
{
	// data[2] --> Wave Type
	if (data[2] >= '1' && data[2] <= '6') // Checking if there is data
		eeprom_write_byte((uint8_t *)10, data[2]);


	// data[6] to data[10] --> Frequency in Hz
	if (data[6] >= '0' && data[6] <= '9') { // Checking if there is data
		eeprom_write_byte((uint8_t *)11, data[6]);
		eeprom_write_byte((uint8_t *)12, data[7]);
		eeprom_write_byte((uint8_t *)13, data[8]);
		eeprom_write_byte((uint8_t *)14, data[9]);
		eeprom_write_byte((uint8_t *)15, data[10]);
	}

	// data[14] to data[17] --> Amplitude
	if (data[14] >= '0' && data[14] <= '9') { // Checking if there is data
		eeprom_write_byte((uint8_t *)16, data[14]);
		eeprom_write_byte((uint8_t *)17, data[15]);
		eeprom_write_byte((uint8_t *)18, data[16]);
		eeprom_write_byte((uint8_t *)19, data[17]);
	}

	// data[21] to data[24] --> Offset
	if (data[22] >= '0' && data[21] <= '9' 
	&& (data[21] == '-' || data[21] == '+')) { // Checking if there is data
		eeprom_write_byte((uint8_t *)20, data[21]);
		eeprom_write_byte((uint8_t *)21, data[22]);
		eeprom_write_byte((uint8_t *)22, data[23]);
		eeprom_write_byte((uint8_t *)23, data[24]);
	}

	if (data[28] >= '1' && data[28] <= '2')
		eeprom_write_byte((uint8_t *)28, data[28]);
};

// Reads and stores the EEPROM Data Configurations
void read_data(struct WaveConfig *configuration)
{
	// address 10 --> Wave Type
	char type[2] = {eeprom_read_byte((uint8_t *)10)};

	// address 11 to 15 --> Frequency in Hz
	char frequency[6] = {eeprom_read_byte((uint8_t *)11), eeprom_read_byte((uint8_t *)12),
						 eeprom_read_byte((uint8_t *)13), eeprom_read_byte((uint8_t *)14),
						 eeprom_read_byte((uint8_t *)15)};

	// data[14] to data[17] --> Amplitude
	char amplitude[5] = {eeprom_read_byte((uint8_t *)16), eeprom_read_byte((uint8_t *)17),
						 eeprom_read_byte((uint8_t *)18), eeprom_read_byte((uint8_t *)19)};

	// data[21] to data[24] --> Offset
	char offset[5] = {eeprom_read_byte((uint8_t *)20), eeprom_read_byte((uint8_t *)21),
					  eeprom_read_byte((uint8_t *)22), eeprom_read_byte((uint8_t *)23)};

	char onoff[5] = {eeprom_read_byte((uint8_t *)28)};

	strcpy(configuration->Type, type);
	strcpy(configuration->Freq, frequency);
	strcpy(configuration->Ampl, amplitude);
	strcpy(configuration->Offset, offset);
	strcpy(configuration->State, onoff);
};

void store_type(char *value)
{
	// data[2] --> Wave Type
	eeprom_write_byte((uint8_t *)10, value[0]);
}

void store_frequency(char *value)
{
	if (value[0] != '?') { // Checking if there is data
		eeprom_write_byte((uint8_t *)11, value[0]);
		eeprom_write_byte((uint8_t *)12, value[1]);
		eeprom_write_byte((uint8_t *)13, value[2]);
		eeprom_write_byte((uint8_t *)14, value[3]);
		eeprom_write_byte((uint8_t *)15, value[4]);
	}
}

void store_amplitude(char *value)
{
	if (value[0] != '?') { // Checking if there is data
		eeprom_write_byte((uint8_t *)16, value[0]);
		eeprom_write_byte((uint8_t *)17, value[1]);
		eeprom_write_byte((uint8_t *)18, value[2]);
		eeprom_write_byte((uint8_t *)19, value[3]);
	}
}

void store_offset(char *value)
{
	// data[21] to data[24] --> Offset
	if (value[0] != '?') { // Checking if there is data
		if (value[0] == '-') {
			eeprom_write_byte((uint8_t *)20, value[0]);
			eeprom_write_byte((uint8_t *)21, value[1]);
			eeprom_write_byte((uint8_t *)22, value[2]);
			eeprom_write_byte((uint8_t *)23, value[3]);
		} else {
			eeprom_write_byte((uint8_t *)20, '+');
			eeprom_write_byte((uint8_t *)21, value[0]);
			eeprom_write_byte((uint8_t *)22, value[1]);
			eeprom_write_byte((uint8_t *)23, value[2]);
		}
	}
}

void store_state(char *value)
{
	if (value[0] != '?')
		eeprom_write_byte((uint8_t *)28, value[0]);
}

// Turns the ascii char to its int value
// Only works with 0 to 9.
int char_to_digit(char value)
{
	if (value < '0' || value > '9')
		return 42;
	else
		return value - '0';
}

// Writes the brightness to EEPROM address 30.
void store_brightness(char level)
{
	if (level > '5' || level < '0')
		return;
	eeprom_write_byte((uint8_t *)30, level);
}

// Reads the wave output state
char read_state(void)
{
	return eeprom_read_byte((uint8_t *)28);
}

// Returns the eeprom backlight brightness level at address 30.
char read_brightness(void)
{
	char level = eeprom_read_byte((uint8_t *)30);
	if (level < '0' || level > '5') // If the byte does not have a real value.
		return '3';
	else
		return level;
}

void send_data_to_gui(void)
{
	char buffer[100];
	// Wave Type
	char type = eeprom_read_byte((uint8_t *)10);
	sprintf(buffer, "T=%c=F=", type);

	// Frequency
	for (int i = 0; i < 5; i++) {
		char freq = eeprom_read_byte((uint8_t *)(11 + i));
		sprintf(buffer + strlen(buffer), "%c", freq);
	}

	strcat(buffer, "=A=");

	// Amplitude
	for (int i = 0; i < 4; i++) {
		char ampl = eeprom_read_byte((uint8_t *)(16 + i));
		sprintf(buffer + strlen(buffer), "%c", ampl);
	}

	strcat(buffer, "=O=");

	// Offset
	for (int i = 0; i < 4; i++) {
		char offset = eeprom_read_byte((uint8_t *)(20 + i));
		sprintf(buffer + strlen(buffer), "%c", offset);
	}

	strcat(buffer, "\n"); // add "\n" to show the end

	// send data
	uart_puts(buffer);
}

// Checks to see if the current set up causes clipping.
char clip_check(struct WaveConfig configuration)
{
	float offset;
	if (atof(configuration.Offset) < 0)
		offset = atof(configuration.Offset) * -1.0;
	else
		offset = atof(configuration.Offset);

	float result = atof(configuration.Ampl) + offset;
	if (result > 5)
		return 1;
	else
		return 0;
}

// Check if the EEPROM initialised for the first time
char eeprom_validity(void)
{
	// Checks the first 50 bytes of the eeprom theres data in it.
	for (int i = 0; i < 50; i++) {
		if (eeprom_read_byte((uint8_t *)i) != 0xFF)
			return 1;
	}
	
	// If it reaches here, the eeprom is not initialised
	// Use default values and initialise the eeprom.
	store_type("1");
	store_amplitude("2.50");
	store_frequency("10000");
	store_offset("0.0");
	store_state("1");
	return 0;
}