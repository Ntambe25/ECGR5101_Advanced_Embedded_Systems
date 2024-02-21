/*
 * Nahush Dilip Tambe & Bharath Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 3
 *
 * For Lab 3, ADC (Analog to Digital Conversion) values will be used
 * to Display HEX Digits (0-F) as a Forever Running Cycle.
 *
 * Analog values will be converted to Digital using the formula
 * Vdigital = (Vin * ((2^N)-1))/(3.3), where N is the # of Bits
 */

#include <msp430.h>
#include <stdint.h>    /* Using Standard Library for Mathematics Calculations*/

// Variable to Keep track of Time (ms)
unsigned int OFCount;

// Declaring an Array of Hex Values (0x--)
int Display_Array[16]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};

void initTimer_A(void);       /* Declaring Function to Initialize Timer*/
int ADC_Conversion(int n);    /* Declaring Function to Convert Analog to Digital*/

int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer

    // Set MCLK = SMCLK = 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    initTimer_A();           // Initializing Timer
    _enable_interrupt();     // Enabling Interrupt

    TACCR0 = 1000 -1;

    P2DIR = 0xFF;             // Configuring P2.0 as OUTPUT (7 Segment LED)
    P2SEL = 0x00;             // Setting all Pins on P2 as Outputs

    P1SEL |= BIT3;            // Setting P1.3 as Analog Input

    ADC10AE0 |= 0x08;         // Selecting Channel A0

    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;
    ADC10CTL1 = INCH_3;

    while(1){       // A Forever running While Loop
        ADC10CTL0 |= ENC + ADC10SC;
        ADC_Conversion(ADC10MEM);
    }
}
/*
 * Function to Convert Analog Values to Digital
 * and then Display the HEX digit Accordingly
 */
int ADC_Conversion(int n){

    int V_Input = (n/64);/*scales down value in the range 0f 0-1023 to 0-15*/
    int val =(int)V_Input;
    P2OUT = Display_Array[val];/*displays digit from the array based on scaled down value*/
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
    // IF Loop to cycle through OfCount (ms)
    if(OFCount>=500){
            OFCount = 0;
     }
}
