#include <msp430.h>				
/**
 * Nahush D. Tambe
 * ECGR 5101 - Advanced Embedded Systems - Lab # 1
 *
 * For this Lab, externally wired 7 Segment LED will be controlled
 * using a Button Switch, also externally wired, as follows:
 * By Default, the LED is OFF at Start.
 *
 * Button Press 1: LED is ON Continuously.
 * Button Press 2: LED blinks at a Frequency of 1.0Hz, duty cycle of 50%.
 * Button Press 3: LED is Completely OFF.
 *
 * The process will be repeated Forever. (While Loop)
 */


// Global Variable to use in both, the Main Loop and the Custom-Made Function
// The variable "buttonPress" keeps track of the # of Button Presses
int buttonPress = 0;

// Declaring the Custom-Made Function to tackle Debouncing Issue
void buttonState();

// Start Main Loop
void main(void){
    WDTCTL = WDTPW | WDTHOLD;     // Stop watchdog timer
    P2DIR = BIT0;                 // Configuring P2.0 as OUTPUT (7 Segment LED)

    P1REN = BIT4;                 // Configuring and Enabaling the -
    P1OUT = BIT4;                 // Port 1.4 Resistor for Button to Work

    P2OUT = 0x01;                 // By default ON, turning the LED OFF Manually

    while(1){
        buttonState();            // Checking for a Button Press

        // Start Switch Statement
        switch(buttonPress){
            // Case 1: Button Press 1 -> LED is ON Continuously
            case 1:
                buttonState();                 // Checking for a Button Press
                if(buttonPress == 1){
                    P2OUT &= ~BIT0;            // Turning ON the LED
                    break;
                }
                // Multiple IF/ ELSE IF Statements added to tackle Debouncing

                else if(buttonPress == 2){     // Checking if Button Press = 2
                    P2OUT &= ~BIT0;            // Turning ON the LED
                    __delay_cycles(500000);
                    buttonState();             // Checking for a Button Press
                    if(buttonPress == 3){
                        buttonState();         // Checking for a Button Press
                        P2OUT = 0x01;
                        break;
                     }
                    P2OUT = BIT0;              // Turning OFF the LED
                    __delay_cycles(500000);
                }
                else if(buttonPress == 3){
                    P2OUT = 0x01;
                    break;
                }
            // Case 2: Button Press 2 -> LED is Blinking at 50 % Duty Cycle
            case 2:
                if(buttonPress == 3){
                    buttonState();            // Checking for a Button Press
                    P2OUT = 0x01;
                    break;
                }
                else{
                     buttonState();           // Checking for a Button Press
                     if(buttonPress == 2){
                         P2OUT &= ~BIT0;      // Turning ON the LED
                         __delay_cycles(500000);
                         buttonState();       // Checking for a Button Press
                         if(buttonPress == 3){
                             buttonState();   // Checking for a Button Press
                             P2OUT = 0x01;
                             break;
                         }
                         P2OUT = BIT0;       // Turning OFF the LED
                         __delay_cycles(500000);
                     }
                     buttonState();          // Checking for a Button Press
                     break;
                }
            // Case 3: Button Press 3 -> LED is OFF
            case 3:
                buttonState();               // Checking for a Button Press
                P2OUT = 0x01;                // Turning OFF the LED
                break;
        }
    }
}

// Defining the Custom-Made Function to tackle Debouncing
// Function adds a Delay, Listens for a Button Press, and Updates
// the "buttonPress" Variable accordingly
void buttonState(){
    if((P1IN & BIT4) != BIT4){
        buttonPress++;                      // Updates the Variable
        __delay_cycles(1000000);
        if (buttonPress > 3){
        buttonPress = 1;
        }
    }
}






