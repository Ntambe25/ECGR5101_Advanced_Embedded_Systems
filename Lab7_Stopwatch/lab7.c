/*
 * Nahush Dilip Tambe & Bharath Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 7
 *
 * For Lab 7, a Stopwatch was Implemented using a Quad Digit LED Display
 *
 * Initially, the "00.00" will be displayed. With 1st Button Press,
 * Stopwatch will start running. 2nd Button Press, it will stop,
 * and 3rd Button Press will reset it back to "00.00".
 *
 */
#include <msp430.h>

// Declaring an Array of Hex Values (0x--) (0 - 9)
int Display_Array[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// Global Variables Declared for Control and Display
int i = 0, val = 0, x, k = 0;

// Array to Store Digits for Display
int digit[4] = {0, 0, 0, 0};

// Array to Store Positions of Display Digits
int pos[4] = {BIT4, BIT5, BIT6, BIT7};

// Prototype Functions Declaration
void initSystem();
void initTimer_A(void);
void Digits_Buffer(int val);

// Start Main Function
void main(void) {
    initSystem();       // Initializing System with Registers
    initTimer_A();      // Initializing Timer for Stopwatch and Display

    //Enabling Global Interrupts
    _enable_interrupt();

    TACCR0 = 5000;

    while (1) {
        // Different Action based on # of Button Presses
        // 3rd Button Press, Reset Display to "00.00"
        if (i % 3 == 0) {
            val = 0;
            Digits_Buffer(val/2);
        }
        // 1st Button Press, Starts the Stopwatch
        if (i % 3 == 1) {
            Digits_Buffer(val/2);
        }
        // 2nd Button Press, Stops the Stopwatch and Display Current Value
        if (i % 3 == 2) {
            Digits_Buffer(x);
        }
    }
}

/*
 * Function to Initialize System Registers
 */
void initSystem(){
    WDTCTL = WDTPW + WDTHOLD;  // Stopping Watchdog Timer
    BCSCTL1 = CALBC1_1MHZ;     // Set System Clock to 1MHz
    DCOCTL = CALDCO_1MHZ;

    // Setting Output PINS
    // Port 2.0 through 2.7 --> segments a, b, c, d, e,f, g, dp
    // P1.4, 1.5, 1.6, 1.7 --> Digit1, Digit2, Digit3, Digit4
    P1DIR = BIT4 + BIT5 + BIT6 + BIT7;

    P2SEL = 0x00;              //All of Port 2 as output
    P2DIR = 0xFF;

    //Setting Input P1.3 (Onboard Button) and Port 1 Interrupt
    P1IES &= ~BIT3;
    P1IFG &= ~BIT3;
    P1IE |= BIT3;
}
/*
 * Function to Initialize Timer
 */
void initTimer_A(void){
    TACCR0 = 0;         // Initializing the TACCR0 Register
    TACCTL0 |=CCIE;
    TACTL = TASSEL_2 + ID_0 + MC_1;
}

/*
 * Function to Break a Value into Individual Digits for Display
 */
void Digits_Buffer(int val) {
    // Storing all 4 Digits into an Array
    digit[0] = Display_Array[(val) % 10];
    digit[1] = Display_Array[(val / 10) % 10];
    digit[2] = Display_Array[(val / 100) % 10];
    digit[3] = Display_Array[(val / 1000)];
}

/*
 * Timer A0 ISR for the Stopwatch and the Display
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void) {
    val++;              // Incrementing the "val" Variable

    // If Stopwatch value reaches "99.99", Reset it back to "00.00"
    if(val == 9999)val=0;

    // Updating the Display
    if(k < 4) {
        if (k == 2)
            // Keeping the Decimal Point of Digit3 always ON
            P2OUT = digit[k] - 0x80;
        else
            P2OUT = digit[k];
        P1OUT = pos[k];
        k++;
        if(k==4)k=0;
    }
}

/*
 * Port ISR for the Button
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1IFG &= ~BIT3;     // Clearing the Port 1 Interrupt Flag
    i = i + 1;
    // Value passed to Display Function when Stopwatch is Stopped
    x = val/2;
    P2OUT = 0xFF;
}


