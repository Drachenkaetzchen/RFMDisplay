#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "rfm12.h"

#define MASK1(b1)                         ( (1<<b1) )
#define MASK2(b1,b2)                      ( (1<<b1) | (1<<b2) )
#define MASK3(b1,b2,b3)                   ( (1<<b1) | (1<<b2) | (1<<b3) )
#define MASK4(b1,b2,b3,b4)                ( (1<<b1) | (1<<b2) | (1<<b3) | (1<<b4) )
#define QUARTER_SECOND_TICKS 64
#define OE PB2
#define MR PD6
#define LATCH PD5
#define CYCLE PD0
#define DATA PB0
#define CLOCK PB4

/**
 * Defines the number of buffers to fill. Each buffer eats 4 bytes of RAM.
 */
#define NUM_BUFFER_COUNT 2

/**
 * As we have a static display, we need to cycle between polarities. Holds the polarity.
 */
volatile uint8_t cycle = 0;

/**
 * Global counter for the shift register. Represents the current bit to output
 */
volatile uint8_t gcnt = 0;

/**
 * Holds the 32 bit number to display
 */
volatile uint32_t numberToDisplay = 0;

void pushToOutput (uint8_t cnt);

/**
 * Pushes a specific bit to the output. This function is designed to run as fast as possible,
 * as we need to listen for interrupts. See the ISR for TIMER1_COMPA_vect
 */
void pushToOutput (uint8_t cnt) {
	if (cycle) {
		if (!!(numberToDisplay & (1UL << (31 - cnt)))) {
		 PORTB |= (1 << DATA);
	   } else {
		  PORTB &= ~(1<<DATA);
	   }

	   PORTB |= (1 << CLOCK);
	   PORTB &= ~(1 << CLOCK);
	} else {
		if (!!(~numberToDisplay & (1UL << (31 - cnt)))) {
			 PORTB |= (1 << DATA);
		   } else {
			PORTB &= ~(1<<DATA);
		   }
		   PORTB |= (1 << CLOCK);
		   PORTB &= ~(1 << CLOCK);
		}

}

/**
 * Timer1 ISR.
 *
 * Each time this gets called, it outputs a single bit to the shift register to keep the cycles low, in case
 * something is received via the RFM12 module.
 *
 * Whent he cycle is full, we latch the shift register.
 */
ISR(TIMER1_COMPA_vect)
{
	if (gcnt==0) {
		PORTD &= ~(1 << LATCH);
	}
	pushToOutput(gcnt);
	gcnt++;

	if (gcnt==32) {
		gcnt=0;

		PORTD |= (1 << LATCH);
		if (cycle) {
			PORTD &= ~(1 << CYCLE);
		} else {
			PORTD |= (1 << CYCLE);
		}
		cycle = !cycle;
	}
}

/**
 * Set up the ports
 */
void setup (void) {
	  DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD0);
	  DDRB |= (1 << PB4) | (1 << PB0);

	  DDRD &= ~(1 << PD2);
	  DDRB &= ~(1 << PB1);

	  PORTB &= ~(1 << OE);
	  PORTD |= (1 << MR);

	  cycle = 0;

	  cli(); // Disable global interrupts


	  // Turn off Input Capture Noise Canceler, Input Capture Edge Select on Falling, stop the clock
	  TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
	  // Disconnect the timer from the output pins, Set Waveform Generation Mode to Normal
	  TCCR1A = (0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
	  // Reset the count to zero
	  TCNT1 = 0;
	  // Set the output compare registers to zero
	  OCR1A = 0;
	  OCR1B = 0;
	  // Clear the input capture?
	  // ICR1 = 0;
	  // Disable all Timer1 interrupts
	  TIMSK &= ~(1 << TOIE1 | 1 << OCIE1A | 1 << OCIE1B | 1 << ICIE1);
	  // Clear the Timer1 interrupt flags
	  TIFR |= MASK4(TOV1,OCF1A,OCF1B,ICF1);

	  OCR1A = QUARTER_SECOND_TICKS;

	  TCCR1A = (TCCR1A & ~MASK2(WGM11,WGM10)) | (((4 & 3) >> 0) << WGM10);
	  TCCR1B = (TCCR1B & ~MASK2(WGM13,WGM12)) | (((4 & 12) >> 2) << WGM12);

	  TCCR1B |= (1 << CS11);
	  TCCR1B &= ~(1 << CS10);
	  TCCR1B &= ~(1 << CS12);

	  TIMSK |= (1<<OCIE1A);
	  sei(); // Enable global interrupts

	  numberToDisplay = 0;

	  rfm12_power_up();

	  rfm12_rx_clear();
}

int main ( void )
{
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12

	setup();
	loop();
}

void loop() {
	uint8_t *bufptr;
	uint8_t i;
	numberToDisplay=983280;

	uint32_t numBuffer[NUM_BUFFER_COUNT];
	uint8_t numBufferCount = 0;
	uint8_t numBufferOK = false;

	while (1) {

		if (rfm12_rx_status() == STATUS_COMPLETE)
		{
			bufptr = rfm12_rx_buffer(); //get the address of the current rx buffer


			numBuffer[numBufferCount]= (uint32_t)strtoul((char*)bufptr, NULL, 24);
			numBufferCount++;
			rfm12_rx_clear();

			// As I didn't want to change the code on the sender node, I simply wait until
			// all buffers have the same number. This may or may not be a good solution,
			// but it works pretty well in my case.

			if (numBufferCount == NUM_BUFFER_COUNT) {
				numBufferCount = 0;
			}

			numBufferOK = true;
			for (i=1;i<NUM_BUFFER_COUNT;i++) {
				if (numBuffer[i] != numBuffer[0]) {
					numBufferOK = false;
				}
			}


			if (numBufferOK) {
				numberToDisplay = numBuffer[0];

				rfm12_power_down();

				clock_prescale_set(clock_div_32); // Lower the clock frequency

				// Wait for approx. 10 minutes
				for (i=0;i<80;i++) {
				_delay_ms(1000);
				}

				i=0;
				// Prescale to normal, power up everything again
				clock_prescale_set(clock_div_8);
				rfm12_power_up();

			}

		}
	rfm12_tick();
	_delay_ms(1);
	}
}

