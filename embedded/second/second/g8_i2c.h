/*
 * g8_i2c.h
 *
 * Created: 7/05/2024 1:21:49 PM
 *  Author: marcu
 */

#ifndef G8_I2C_H_
#define G8_I2C_H_

// Initialise the i2c
void i2c_init(void);

// Starts the i2c communication
uint8_t i2c_start(char write_address);

// Performs a repeated start 
uint8_t i2c_repeat_start(char read_address);

// Writes data through the i2c
uint8_t i2c_write(char data);

// Sends a ACK bit read
char i2c_read_ack(void);

// Sends a NACK bit read
char i2c_read_nack(void);

// Stops the i2c communcation
void i2c_stop(void);

// Initialises as a slave
void i2c_slave_init(uint8_t slave_address);

// Listens to the i2c bus
int8_t i2c_slave_listen(void);

// Transmits something as a slave
int8_t i2c_slave_transmit(char data);

// Receives communications as a slave
char i2c_slave_receive(void);

#endif /* G8_I2C_H_ */