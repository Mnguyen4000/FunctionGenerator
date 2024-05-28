/*
 * g8_DAC.h
 *
 * Created: 4/04/2024 12:01:20 PM
 *  Author: marcu
 */

#ifndef G8_DAC_H_
#define G8_DAC_H_

// Initialises the SPI DAC.
void DAC_SPI_init(void);

// Initialises the DAC
void DAC_init(void);

// Sends a voltage out via I2C DAC
void voltage_out(int value);

// Sends a voltage out via SPI DAC
void voltage_out2(uint16_t value);

// Generates a Sine wave through the DAC
void signal_sinusoidal(char *frequencyStr);

// Produces a sawtooth wave through the DAC
void signal_sawtooth(char *frequencyStr);

// Produces a Reverse Sawtooth Wave through the DAC
void signal_r_sawtooth(char *frequencyStr);

// Produces a Triangle Wave through the DAC
void signal_triangle(char *frequencyStr);

// Produces a Square Wave with a 50% duty cycle through the DAC
void signal_square(char *frequencyStr);

// Creates a DC Signal through the DAC
void signal_dc(char *frequencyStr);
#endif /* G8_DAC_H_ */