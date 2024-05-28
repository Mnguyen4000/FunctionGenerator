/*
 * g8_brightness.h
 *
 * Created: 28/03/2024 12:31:33 PM
 *  Author: marcu
 */

#ifndef G8_BRIGHTNESS_H_
#define G8_BRIGHTNESS_H_

// Getter Function for the brightnessValue.
int get_brightness(void);

// Initialises backlight and pin interrupts.
void backlight_init(void);

// Adjusts the brightness depending on the percentage
void brightness(int percentage);

// Adjusts the brightness depending on the levels 1 to 5
void brightness_level(int level);

// Changes the brightness level according to the current EEPROM.
void update_backlight(void);
#endif /* G8_BRIGHTNESS_H_ */