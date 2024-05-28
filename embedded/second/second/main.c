/*
 * second.c
 *
 * Created: 15/05/2024 3:47:30 PM
 * Author : marcu
 */
#include <util/delay.h>
#include <avr/io.h>
#include "g8_DAC.h"
#include "g8_i2c.h"


int main(void)
{
	// Initialise the I2C and DAC
	DAC_SPI_init();
	DAC_init();
	DDRC |= (1 << 3);
	DDRB &= ~(1 << 1);
	while (1) {
		
		
		// Toggles BNC connector
		PORTC |= (1 << 3);
		PORTC &= ~(1 << 3);

		// Does a signal sawtooth with the DAC.
		if (PINB & (1 << 1))
			signal_triangle("20000");
	}
}
