/*
 * Nahush Dilip Tambe & Bharath Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 4
 *
 * For Lab 4, ADC (Analog to Digital Conversion) values will be used
 * to Display Digital Values based on Analog Input.
 *
 * A buffer will be designed to represent a range of Scaled Voltage
 * values on a Multiplexed Quad Digit Display.
 */

#include <msp430.h>
#include <stdint.h>           // Using Standard Library for Mathematics Calculations

// Declaring an Array of Hex Values (0x--)
int Display_Array[11]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8, 0x80, 0x90, 0xBF};

void ADC_Conversion(int n);  /* Function to Convert Analog Values */
void dig(int pos,int val);   /* Function to Display the correct Digital Values */
void initTimer(void);        /* Declaring Function to Initialize Timer*/

// Global Variables Declared for Buffer
int flag = 0; int oldavg = 0; int sum = 0;

int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer

    // Configuring P2.0 as OUTPUT (7 Segment LED)
    P2DIR = 0xFF; P2SEL = 0x00;
    P1SEL |= BIT3;            // Setting P1.3 as Analog Input

    // Setting P1.4, 5, 6, 7 as Outputs (Powers for Quad Digits)
    P1DIR |= BIT4 | BIT5 | BIT6 | BIT7;

    ADC10AE0 |= 0x01; ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;
    ADC10CTL1 = INCH_3;

    initTimer();
    __enable_interrupt();

    while(1);                  // A Forever running While Loop
}
/*
 * Function to Convert Analog Values to Digital
 * and then Display the Digital Values
 */
void ADC_Conversion(int val){
    val = (val-512);          // Scaling Values between -512 - +511
    int sign;                 // Variable to store Sign Info.
    if (val<0){
        sign = 1; val=-val;   // If Neg. #, then sign = 1, and Extract Abs. Value
    }
    // Storing the 3 Digits into the Buffer to Display
    int digit1 = val % 10;
    val = val / 10;
    int digit2 = val % 10;
    val = val / 10;
    int digit3 = val % 10;

    if(digit3 == 0 && digit2!=0){
        if (sign==1)dig(0x40,10);   // Digit3 Sign
        dig(0x20,digit2);           // Digit2 Sign
    }
    else if(digit3==0 && digit2==0){
        if (sign==1)dig(0x20,10);   // Digit3 Sign
    }
    else{
        if (sign==1)dig(0x80,10);   // Digit4
        dig(0x20,digit2);           // Digit2
        dig(0x40,digit3);           // Digit3
    }
    dig(0x10,digit1);
}
/*
 * Function to Display the correct Digital Values
 *
 * Function turns the Appropriate Power of the Digit to
 * Display one of the 4 Digits/Negative Sign
 */
void dig(int pos,int val){
    P1OUT = pos; P2OUT = Display_Array[val];
    __delay_cycles(5000);
}
/*
 * Function to Initialize Timer # A
 * Setting for Interrupt Use
 */
void initTimer(void) {
    // Enable interrupt for CCR0
    TACCR0 = 30; TACCTL0 |= CCIE;
    // Selecting SMCLK, SMCLK/1, Up Mode
    TACTL = TASSEL_2 + MC_1 + ID_3 + TACLR;
}
/*
 * Interrupt Function Definition
 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void) {
    // Taking the Average of 7 values
    if(flag<7){
       ADC10CTL0 |= ENC + ADC10SC;
       sum+=ADC10MEM; flag++;
            }
    if(flag==7){
        int avg=sum/7;
        oldavg=avg; flag = 0; sum = 0;
    }
    // Calling the Analog to Digital COnversion Func.
    ADC_Conversion(oldavg);
}
