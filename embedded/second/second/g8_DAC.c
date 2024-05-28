/*
 * g8_DAC.c
 *
 * Created: 4/04/2024 12:01:09 PM
 *  Author: marcu
 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "g8_DAC.h"
#include "g8_i2c.h"

#define SAMPLES_PER_WAVEFORM 4096
#define DAC_ADDR 0x60	 // 0b1100 0000
#define MAX_VOLTAGE 4096 // DAC code ranges from 0 to 4095
#define DAC2_ADDR 0x61

volatile uint8_t isr_flag = 0;
volatile uint16_t sample_count = 0;
volatile uint16_t desired_samples = 0;
volatile uint16_t cc = 0; // Array to hold waveform values
volatile int index1 = 0;  // Index into waveform array

void DAC_SPI_init(void)
{
	DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2); // Setting MOSI, SCK, SS as output
	SPCR |= (1 << SPE) | (1 << MSTR);				 // Enable SPI master mode
	SPSR = (1 << SPI2X);							 // Set clock rate
	PORTB |= (1 << PORTB2);							 // Set SS high initially
}

// Initialises the DAC
void DAC_init(void)
{
	i2c_init(); // Initialises I2C library
}

// Value must be a value from 0 to 4095
void voltage_out(int value)
{
	// Address the DAC
	i2c_start((DAC_ADDR << 1)); // set device address and write mode
	if (value >= MAX_VOLTAGE)
	{
		value = MAX_VOLTAGE - 1;
	}
	i2c_write(0x0F & (value >> 8));
	i2c_write(value & 0xFF);
	i2c_stop();
}

void voltage_out2(uint16_t value)
{
	PORTB &= ~(1 << PORTB2);
	if (value > 4095)
		value = 4095;
		
	uint16_t command = 0x7000 | (value & 0x0FFF); // BUF=1, GA=1, SHDN=1

	// Send the command over SPI
	SPDR = (command >> 8); // Send upper byte
	while (!(SPSR & (1 << SPIF)))
		;
	SPDR = command & 0xFF; // Send lower byte
	while (!(SPSR & (1 << SPIF)))
		;

	PORTB |= (1 << PORTB2);
}

const uint16_t sine_table[30] PROGMEM = {2048, 2473, 2880, 3251, 3569, 3821, 3995, 4084, 4084, 3995,
										 3821, 3569, 3251, 2880, 2473, 2048, 1622, 1215, 844, 526,
										 274, 100, 11, 11, 100, 274, 526, 844, 1215, 1622};

// Generates a Sine wave through the DAC
void signal_sinusoidal(char *frequencyStr)
{

	int freq = atoi(frequencyStr);

	// Determine the period in microseconds
	float cycle = (1 / (float)freq) * 1000000;

	// Get the steps of the radians per microsecond.
	float step = (float)360 / cycle;

	// Initial Value
	float currentDegree = 0;

	// Voltage will increase in steps until it reaches the max
	for (int i = 0; i < cycle; i++)
	{
		voltage_out2(2048 * sin((float)currentDegree * 3.14159 / (float)180) + 2048);
		currentDegree += step;
		if (currentDegree > 360)
		{
			currentDegree = 360;
		}
		else if (currentDegree < 0)
		{
			currentDegree = 0;
		}
	}
}

// Produces a sawtooth wave through the DAC
void signal_sawtooth(char *frequencyStr)
{
	int count = 0;
	while (count < MAX_VOLTAGE) {
		voltage_out2(count);
		count += 1;
	}
}

// Produces a Reverse Sawtooth Wave through the DAC
void signal_r_sawtooth(char *frequencyStr)
{
	
	int count = MAX_VOLTAGE - 1;
	while (count > 0) {
		voltage_out2(count);
		count -= 1;
	}
}

// Produces a Triangle Wave through the DAC
void signal_triangle(char *frequencyStr)
{
	signal_sawtooth("500");
	signal_r_sawtooth("500");
}

// Produces a Square Wave with a 50% duty cycle through the DAC
// Assuming the duty cycle is always 50%
void signal_square(char *frequencyStr)
{
	// timer delay based on frequency
	voltage_out2(MAX_VOLTAGE);
	_delay_us(10);

	// timer delay based on frequency
	voltage_out2(0);
	_delay_us(10);
}

// Creates a DC Signal through the DAC
void signal_dc(char *frequencyStr)
{
	int freq = atoi(frequencyStr);

	// Determine the period in microseconds
	float cycle = (1 / (float)freq) * 1000000;

	// Set voltage High
	voltage_out(MAX_VOLTAGE);

	// Keep voltage High for entire cycle
	for (int i = 0; i < cycle; i++)
	{
		_delay_us(1);
	}
}

void PWM_init(void)
{
	TCCR1A |= (0 << WGM11) | (0 << WGM10);	 // CTC
	TCCR1A |= (1 << COM1A1) | (0 << COM1A0); // clear on output compare
	TCCR1B |= (1 << WGM12) | (0 << WGM13);	 // Set WGM12 bit for CTC mode
	TCCR1B |= (1 << CS10);					 // CS10 bits for prescaler of 1
	OCR1A = 24;								 // sts it to have a frequency of 160kHz
	TIMSK1 |= (1 << OCIE1A);				 // Enable Timer1 output compare match A interrupt
	sei();									 // Set global interrupt
}

ISR(TIMER1_COMPA_vect)
{
	voltage_out(cc);
	cc = (cc + 1) % 4096;

	isr_flag = 1;
}

// mca409201

void signal_r_sawtoothh(char *frequencyStr)
{
	// Convert frequencyStr to an integer
	uint16_t frequency = atoi(frequencyStr);

	// Calculate number of ISR triggers per sample for desired frequency
	uint16_t triggers_per_sample = (160000 / frequency) / SAMPLES_PER_WAVEFORM;

	// Initialize counter
	uint16_t counter = 0;

	// Generate waveform
	while (1)
	{
		// Check if update flag is set
		if (isr_flag)
		{
			// Check if it's time to output a new sample
			if (counter >= triggers_per_sample)
			{
				// Output next value in waveform
				voltage_out(cc);
				cc = (cc + 1) % MAX_VOLTAGE;
				counter = 0; // Reset counter
			}
			else
			{
				counter++; // Increment counter
			}

			// Clear update flag
			isr_flag = 0;
		}
	}
}