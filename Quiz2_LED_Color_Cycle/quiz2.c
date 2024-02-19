#include <msp430.h>				


/**
 * Nahush D. Tambe
 * ECGR 5101 - Advanced Embedded Systems
 *
 * RGB LED Blinking Program
 * The LEDs will be blinked by cycling through various colors using
 * multiple combinations of RGB using a FOR LOOP.
 *
 * The FOR Loop will cycle through 7 states with the use of a switch statement, with a certain LED combination will
 * blink for each of the states. Last state is when all LEDs are OFF.
 *
 * For this Quiz, _delay_cycles() function provided is used.
 *
 *
 */

// Start main Function
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		           // stop watchdog timer
	P2DIR |= 0x02 | 0x08 | 0x64;		       // configure P2.1, P2.3, and P2.5 as output (RED, GREEN, and BLUE LEDs)

	volatile unsigned int i;		           // volatile to prevent optimization

	while(1)
	{
	    // Declaring a variable to loop through color states
        unsigned char color;

        // Start FOR Loop
        for(color = 0; color <= 6; color++){
	        switch(color){
	        case 0:P2OUT = 0x02;                        // Blink ONLY RED LED (Green and Blue are OFF)
	        break;
	        case 1:P2OUT = 0x08;                        // Blink ONLY GREEN LED (Red and Blue are OFF)
	        break;
	        case 2:P2OUT = 0x64;                        // Blink ONLY BLUE LED (Red and Green are OFF)
            break;
	        case 3:P2OUT = 0x02 | 0x08 | 0x64;          // Blink ALL 3 -> White LED
            break;
	        case 4:P2OUT = 0x02 | 0x08;                 // Blink RED + GREEN = YELLOW LED
            break;
	        case 5:P2OUT = 0x02 | 0x64;                 // Blink RED + BLUE = VIOLET LED
            break;
	        case 6:P2OUT = 0x08 | 0x64;                 // Blink GREEN + BLUE = CYAN LED
	        break;
	        }
	        _delay_cycles(500000);     // delay
	    }
        // End FOR Loop


        // Blink NONE -> ALL 3 LEDs are OFF
        P2OUT = 0x00;
        _delay_cycles(1000000);     // delay
	}
}
// End main Function


