/*
 * g8_button.h
 *
 * Created: 19/04/2024 2:43:21 PM
 *  Author: marcu
 */

#ifndef G8_BUTTON_H_
#define G8_BUTTON_H_

#include "g8_EEPROM.h"

// Initialises the buttons as inputs
void init_buttons(void);

// Checks the states of the buttons if they are pressed
int check_buttons(void);

// Performs actions depending on which button is pressed
void button_operations(struct WaveConfig *data);

// Gets count
int get_count(void);

// Gets Selected parameter
int get_selected(void);
#endif /* G8_BUTTON_H_ */