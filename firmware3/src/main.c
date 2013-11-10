#include "rfm12_config.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "rfm12lib/rfm12.h"

void main (void) {
	_delay_ms(1000);  //little delay for the rfm12 to initialize properly
	rfm12_init();


	while (1) {
		loop();
	}
}


void loop() {
	uint8_t tv[] = "foobar";
	uint16_t ticker = 0;
	ticker ++;
	if(ticker == 3000){
		ticker = 0;

		rfm12_tx (sizeof(tv), 0, tv);
	}



	rfm12_tick();

	_delay_us(100); //small delay so loop doesn't run as fast
}

