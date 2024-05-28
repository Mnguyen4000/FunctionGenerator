/*
 * g8_pot.h
 *
 * Created: 17/04/2024 3:47:49 PM
 *  Author: marcu
 */

#ifndef G8_POT_H_
#define G8_POT_H_

// Initialises the SPI communication
void spi_init(void);

// Transmits the message through SPI
void spi_transmit(uint8_t data);

// Alter the Digital Pot1 through SPI
void resistor1(uint8_t value);

// Alter the Digital Pot2 through SPI
void resistor2(uint8_t value);

#endif /* G8_POT_H_ */