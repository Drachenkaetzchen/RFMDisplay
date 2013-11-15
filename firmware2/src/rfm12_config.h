/**** RFM 12 library for Atmel AVR Microcontrollers *******
 * 
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * @author Peter Fuhrmann, Hans-Gert Dahmen, Soeren Heisrath
 */

/******************************************************
 *                                                    *
 *           C O N F I G U R A T I O N                *
 *                                                    *
 ******************************************************/

/*
	Connect the RFM12 to the AVR as follows:

	RFM12           | AVR
	----------------+------------
	SDO             | MISO
	nIRQ            | INT0
	FSK/DATA/nFFS   | VCC
	DCLK/CFIL/FFIT  |  -
	CLK             |  -
	nRES            |  -
	GND             | GND
	ANT             |  -
	VDD             | VCC
	GND             | GND
	nINT/VDI        | -
	SDI             | MOSI
	SCK             | SCK
	nSEL            | Slave select pin defined below
*/


/************************
 * PIN DEFINITIONS
 */

//Pin that the RFM12's slave select is connected to
#define DDR_SS DDRD
#define PORT_SS PORTD
#define BIT_SS 3

#define DDR_SPI_SS DDRD
#define PORT_SPI_SS PORTD

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define PIN_SPI PINB
#define BIT_MOSI 5
#define BIT_MISO 6
#define BIT_SCK  7
#define BIT_SPI_SS 3

//this is the hardware SS pin of the AVR - it 
//needs to be set to output for the spi-interface to work 
//correctly, independently of the CS pin used for the RFM12


#define RFM12_BASEBAND RFM12_BAND_868
#define RFM12_FREQUENCY       868170000UL
#define FSK_SHIFT             125000
#define RFM12_RSSI_THRESHOLD  RFM12_RXCTRL_RSSI_79
#define RFM12_FILTER_BW       RFM12_RXCTRL_BW_400
#define RFM12_POWER           RFM12_TXCONF_POWER_0
#define RFM12_LNA_GAIN        RFM12_RXCTRL_LNA_6
#define RFM12_XTAL_LOAD       RFM12_XTAL_11_5PF
#define DATARATE_VALUE        RFM12_DATARATE_CALC_HIGH(9600.0)
#define RFM12_TX_BUFFER_SIZE  0
#define RFM12_RX_BUFFER_SIZE  10


/************************
 * INTERRUPT VECTOR
 * set the name for the interrupt vector here
 */
 
//the interrupt vector
#define RFM12_INT_VECT (INT0_vect)

//the interrupt mask register
#define RFM12_INT_MSK GIMSK

//the interrupt bit in the mask register
#define RFM12_INT_BIT (INT0)

//the interrupt flag register
#define RFM12_INT_FLAG EIFR

//the interrupt bit in the flag register
#define RFM12_FLAG_BIT (INTF0)

//setup the interrupt to trigger on negative edge
#define RFM12_INT_SETUP()   MCUCR |= (1<<ISC01)


/************************
 * FEATURE CONFIGURATION
 */

#define RFM12_LIVECTRL 0
#define RFM12_LIVECTRL_CLIENT 0
#define RFM12_LIVECTRL_HOST 0
#define RFM12_LIVECTRL_LOAD_SAVE_SETTINGS 0
#define RFM12_NORETURNS 1
#define RFM12_NOCOLLISIONDETECTION 1
#define RFM12_TRANSMIT_ONLY 0
#define RFM12_SPI_SOFTWARE 1
#define RFM12_USE_POLLING 0
#define RFM12_RECEIVE_ASK 0
#define RFM12_TRANSMIT_ASK 0
#define RFM12_USE_WAKEUP_TIMER 0
#define RFM12_USE_POWER_CONTROL 1
#define RFM12_LOW_POWER 1
#define RFM12_USE_CLOCK_OUTPUT 0
#define RFM12_PWRMGT_SHADOW 1
#define RFM12_LOW_BATT_DETECTOR 0

/************************
 * UART DEBUGGING
 * en- or disable debugging via uart.
 */
 
#define RFM12_UART_DEBUG 0

#define RFM12_USE_CLOCK_OUTPUT        0
#define RFM12_CLOCK_OUT_FREQUENCY     RFM12_CLOCK_OUT_FREQUENCY_10_00_MHz
