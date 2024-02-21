
/*
 * Nahush Dilip Tambe & Bharat Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 2
 *
 * For Lab 2, Internal Board Interrupt Timer Library was used
 * to Display HEX Digits (0-F) as a Forever Running Cycle.
 * Code Reference from - http://www.ocfreaks.com/msp430-timer-programming-tutorial/
 *
 */

#include <msp430.h>

// Declaring Global Functions
void initTimer_A(void);

// Declaring Global Variables & Array of Hex Values (0x--)
int arr[16]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};

// Variable for Iterating through the Display Array
int j=0;
unsigned int OFCount;

// Start Main Function
int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer

    // Set MCLK = SMCLK = 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    initTimer_A();            // Initializing Timer
    _enable_interrupt();      // Enabling Interrupt

    OFCount = 0;
    TACCR0 = 1000 -1;

    P2DIR = 0xFF;             // Configuring P2.0 as OUTPUT (7 Segment LED)
    P2SEL = 0x00;             // Setting all Pins on P2 as Outputs
    P2SEL2 = 0x00;

    P2OUT = 0xFF;             // Manually Turning LEDs OFF at the Start

    // An Empty While Loop
    while(1);
}

/*
 * Function to Initialize Timer # A
 * Setting for Interrupt Use
 */
void initTimer_A(void){
    TACCR0 = 0;               // Initially, Stopping the Timer
    TACCTL0 |=CCIE;           // Enable interrupt for CCR0

    // Selecting SMCLK, SMCLK/1, Up Mode
    TACTL = TASSEL_2 + ID_0 + MC_1;
}

/*
 * Entered every 1ms, then Counts up to the the
 * number defined in OFCount
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void){
    // Incrementing "OFCount" to keep track for the Delay
    OFCount++;
    // IF Loop to cycle through Hex Digits for Display
    if(OFCount>=500){
            P2OUT=arr[j];
            j=j+1;
            if(j==16)j=0;
            OFCount = 0;
     }
}



