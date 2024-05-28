/*
 * g8_i2c.c
 *
 * Created: 7/05/2024 1:21:37 PM
 *  Author: marcu
 */

#include <avr/io.h>
#include <math.h>
#include "g8_i2c.h"

#define SCL_CLK 100000L
#define BITRATE(TWSR) ((F_CPU / SCL_CLK) - 16) / (2 * pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1)))))

// I2C Initialise Function
void i2c_init(void)
{
	TWBR = BITRATE(TWSR = 0x00); // Sets the I2C Speed and initialises it
}

// I2C Start function
uint8_t i2c_start(char write_address)
{
	uint8_t status;									  /* Declare variable */
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate START */

	while (!(TWCR & (1 << TWINT)))
		;				  /* Wait until TWI finish its current job */
	status = TWSR & 0xF8; /* Read TWI status register */

	if (status != 0x08) /* Check weather START transmitted or not? */
		return 0;		/* Return 0 to indicate start condition fail */

	TWDR = write_address;			   /* Write SLA+W in TWI data register */
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI & clear interrupt flag */

	while (!(TWCR & (1 << TWINT))); /* Wait until TWI finish its current job */

	status = TWSR & 0xF8; /* Read TWI status register */

	if (status == 0x18) /* Check for SLA+W transmitted &ack received */
		return 1;		/* Return 1 to indicate ack received */

	if (status == 0x20) /* Check for SLA+W transmitted &nack received */
		return 2;		/* Return 2 to indicate nack received */
	else
		return 3; /* Else return 3 to indicate SLA+W failed */
}

uint8_t i2c_repeat_start(char read_address) /* I2C repeated start function */
{
	uint8_t status;									  /* Declare variable */
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate start */
	while (!(TWCR & (1 << TWINT))); /* Wait until TWI finish its current job */

	status = TWSR & 0xF8; /* Read TWI status register */

	if (status != 0x10)				   /* Check for repeated start transmitted */
		return 0;					   /* Return 0 for repeated start condition fail */
	TWDR = read_address;			   /* Write SLA+R in TWI data register */
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */

	while (!(TWCR & (1 << TWINT))); /* Wait until TWI finish its current job */

	status = TWSR & 0xF8; /* Read TWI status register */

	if (status == 0x40) /* Check for SLA+R transmitted &ack received */
		return 1;		/* Return 1 to indicate ack received */

	if (status == 0x48)	  /* Check for SLA+R transmitted &nack received */
		return 2; /* Return 2 to indicate nack received */
	else
		return 3; /* Else return 3 to indicate SLA+W failed */

}

uint8_t i2c_write(char data) /* I2C write function */
{
	uint8_t status; /* Declare variable */

	TWDR = data; /* Copy data in TWI data register */

	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */

	while (!(TWCR & (1 << TWINT))); /* Wait until TWI finish its current job */

	status = TWSR & 0xF8; /* Read TWI status register */

	if (status == 0x28) /* Check for data transmitted &ack received */
		return 0;		/* Return 0 to indicate ack received */

	if (status == 0x30)	/* Check for data transmitted &nack received */
		return 1; 		/* Return 1 to indicate nack received */
	else
		return 2; 		/* Else return 2 for data transmission failure */

}

char i2c_read_ack(void) /* I2C read ack function */
{
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); /* Enable TWI, generation of ack */
	while (!(TWCR & (1 << TWINT)));		 /* Wait until TWI finish its current job */
	return TWDR; /* Return received data */
}

char i2c_read_nack(void) /* I2C read nack function */
{
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)));		 /* Wait until TWI finish its current job */
	return TWDR; /* Return received data */
}

void i2c_stop(void) /* I2C stop function */
{
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN); /* Enable TWI, generate stop */
	while (TWCR & (1 << TWSTO)); /* Wait until stop condition execution */
}

void i2c_slave_init(uint8_t slave_address)
{
	TWAR = slave_address;							 /* Assign Address in TWI address register */
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); /* Enable TWI, Enable ack generation */
}

int8_t i2c_slave_listen(void)
{
	while (1)
	{
		uint8_t status; /* Declare variable */
		while (!(TWCR & (1 << TWINT))); /* Wait to be addressed */

		status = TWSR & 0xF8; /* Read TWI status register */

		if (status == 0x60 || status == 0x68) /* Own SLA+W received &ack returned */
			return 0;

		/* Return 0 to indicate ack returned */
		if (status == 0xA8 || status == 0xB0) /* Own SLA+R received &ack returned */
			return 1;						  /* Return 0 to indicate ack returned */

		if (status == 0x70 || status == 0x78)	/* General call received &ack returned */
			return 2; /* Return 1 to indicate ack returned */
		else
			continue; /* Else continue */
	}
}

int8_t i2c_slave_transmit(char data)
{
	uint8_t status;
	TWDR = data; /* Write data to TWDR to be transmitted */

	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); /* Enable TWI & clear interrupt flag */

	while (!(TWCR & (1 << TWINT))); /* Wait until TWI finish its current job */

	status = TWSR & 0xF8; /* Read TWI status register */

	if (status == 0xA0) {						  /* Check for STOP/REPEATED START received */
		TWCR |= (1 << TWINT); /* Clear interrupt flag & return -1 */
		return -1;
	}

	if (status == 0xB8) /* Check for data transmitted &ack received */
		return 0;		/* If yes then return 0 */

	if (status == 0xC0) {		/* Check for data transmitted &nack received */
		TWCR |= (1 << TWINT); /* Clear interrupt flag & return -2 */
		return -2;
	}

	if (status == 0xC8) /* Last byte transmitted with ack received */
		return -3; 	/* If yes then return -3 */
	else			/* else return -4 */
		return -4;
}

char i2c_slave_receive(void)
{
	uint8_t status; /* Declare variable */

	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); /* Enable TWI & generation of ack */

	while (!(TWCR & (1 << TWINT))); /* Wait until TWI finish its current job */

	status = TWSR & 0xF8; /* Read TWI status register */

	if (status == 0x80 || status == 0x90) /* Check for data received &ack returned */
		return TWDR;					  /* If yes then return received data */

	/* Check for data received, nack returned & switched to not addressed slave mode */
	if (status == 0x88 || status == 0x98)
		return TWDR; /* If yes then return received data */

	if (status == 0xA0) {						  /* Check whether STOP/REPEATED START */
		TWCR |= (1 << TWINT); /* Clear interrupt flag & return -1 */
		return -1;
	} else {
		return -2; /* Else return -2 */
	}
}