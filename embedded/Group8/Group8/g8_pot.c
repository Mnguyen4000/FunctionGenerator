/*
 * g8_pot.c
 *
 * Created: 17/04/2024 3:12:47 PM
 *  Author: marcu
 */
#include <avr/io.h>
#include "g8_pot.h"

#define DDR_POT1 PORTC
#define CS_POT1 3
#define DDR_POT2 PORTC
#define CS_POT2 0

#define SPI_DDR DDRB
#define CS PINB2
#define MOSI PINB3
#define MISO PINB4
#define SCK PINB5

void spi_init(void)
{
	// set CS, MOSI and SCK to output
	SPI_DDR |= (1 << CS) | (1 << MOSI) | (1 << SCK);

	// enable SPI, set as master, and clock to fosc/128
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);

	// Creating CS for the GPIO pins are outputs
	DDRC |= (1 << CS_POT1) | (1 << CS_POT2);
}

void spi_transmit(uint8_t data)
{
	// load data into register
	SPDR = data;

	// Wait for transmission complete
	while (!(SPSR & (1 << SPIF)));
}

void resistor1(uint8_t value)
{
	// Drive the CS pin low to use it
	DDR_POT1 &= ~(1 << CS_POT1);

	spi_transmit(0b00010001); // Command byte: Write data to data byte, use pot 1
	spi_transmit(value);

	// Deselect the CS pin by driving high
	DDR_POT1 |= (1 << CS_POT1);
}

void resistor2(uint8_t value)
{
	// Drive the CS pin low to use it
	DDR_POT2 &= ~(1 << CS_POT2);

	spi_transmit(0b00010001); // Command byte: Write data to data byte, use pot 1
	spi_transmit(value);

	// Deselect the CS pin by driving high
	DDR_POT2 |= (1 << CS_POT2);
}