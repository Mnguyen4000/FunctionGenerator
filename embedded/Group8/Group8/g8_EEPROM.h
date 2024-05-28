/*
 * g8_EEPROM.h
 *
 * Created: 28/03/2024 10:12:23 AM
 *  Author: marcu
 */

#ifndef G8_EEPROM_H_
#define G8_EEPROM_H_

struct WaveConfig
{
	char Type[10];	 // Wave type
	char Ampl[10];	 // Wave Amplitude (0 to 5V)
	char Freq[10];	 // Wave Frequency (1 to 20000Hz)
	char Offset[10]; // Wave Offset (-5 to 5V)
	char State[10];	 // Wave state (0 or 1 to indicate off or on)
};

// Stores the complete data structure from the string
void store_data(char *data);

// Stores the type data onto EEPROM
void store_type(char *value);

// Stores the frequency data onto EEPROM
void store_frequency(char *value);

// Stores the amplitude data onto EEPROM
void store_amplitude(char *value);

// Stores the offset data onto EEPROM
void store_offset(char *value);

// Reads and stores the EEPROM data to WaveConfig Struct
void read_data(struct WaveConfig *configuration);

// Converts ascii '0' to '9' to int 0 to 9
int char_to_digit(char value);

// Writes the brightness to EEPROM address 30.
void store_brightness(char level);

// Reads the eeprom state.
char read_state(void);

// Returns the stored eeprom brightness value.
char read_brightness(void);

// Converts current eeprom configs to string format.
char *get_string_eeprom(void);

// Sends data to gui
void send_data_to_gui(void);

// Checks if clipping occurs.
char clip_check(struct WaveConfig configuration);

// Checks if the EEPROM is empty or not. Will initialise it if its uninitialised.
char eeprom_validity(void);

#endif /* G8_EEPROM_H_ */