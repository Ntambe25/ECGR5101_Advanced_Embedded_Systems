/*
 * Nahush Dilip Tambe & Bharat Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 6
 *
 * For Lab 6, An Ultrasonic Sensor, Connected to 1st Board
 *  will be Used to Measure the
 * Distance of an Object, then the Value will be Passed
 * over the UART to the 2nd Board and then Displayed
 * on a connected Quad Digit Display.
 */

#include <msp430.h>
#include <stdint.h>

// Declaring an Array of Hex Values (0x--)
int Display_Array[10]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
int digit[3],pos[4] = {BIT4, BIT5, BIT6, BIT7};

#define TRIG_PIN BIT4 // TRIG pin of HC-SR04 connected to P1.4
#define ECHO_PIN BIT3 // ECHO pin of HC-SR04 connected to P1.3

// Declaring Global Variables for Loops, Functions and ISRs
int flag = 0, milisec, sensorVal, k = 0;

// Declaring Prototype Functions
// Functions to Initialize Timers A0
void initTimer_A0(void);

void UART_Setup();      /* Function to Setup UART Registers */
void buffer(int n);     /* Buffer Function to Store a Value as Individual Digits */
void Setup_tx();        /* Setup Function for the Transmitter */
void Setup_rx();        /* Setup Function for the Receiver */
void getdistance();     /* Function to Get Distance from US Sensor (through ISR) */
void Program_rx();      /* Programming Function for the Receiver */
void Program_tx();      /* Programming Function for the Transmitter */

// Functions to Handle Receiving and Transmitting Ends
void uartTransmitData(int i);
int uartReceiveData();

// Start Main Function
int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer

    // Initializing Timer A0 and Enabling Global Interuupts
    initTimer_A0();
    _enable_interrupt();
    UART_Setup();


    // Checking is P1.0 is Connected to VCC or GND
    // IF GND, the Board is Tx, and if VCC, then Boards is Rx
    if(!(P1IN & BIT0)) Setup_tx();
    else Setup_rx();

    // An Infine While Loop to Keep Program Running Forever
    while(1){
        if(!(P1IN & BIT0)) Program_tx();
        else Program_rx();
    }
}

/*
 * Function to set up UART Communication and Required Registers
 */
void UART_Setup(){
    BCSCTL1 = CALBC1_1MHZ;DCOCTL = CALDCO_1MHZ;
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    UCA0CTL1 |= (UCSWRST | UCSSEL_2);
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;
}

/*
 * Function to Convert an Integer Value to 4-digit Display Values
 */
void buffer(int n){
    // An Array called "digit" to store 4 Individual Digits
    digit[0]=Display_Array[n%10];
    if(n < 10) digit[1] = 0xFF;
    else digit[1] =Display_Array[(n/10)%10];
    if(n < 100) digit[2] = 0xFF;
    else digit[2]=Display_Array[(n/100)%10];
}

/*
 * Function to Set Up the Receiver Mode
 */

void Setup_rx(){
    P2DIR = 0xFF;       // Configuring P2.0 as OUTPUT (7 Segment LED)
    P2SEL = 0x00;       // Setting all Pins on P2 as Outputs

    // Setting P1.4, 5, 6, 7 as Outputs
    P1DIR |= BIT4 | BIT5 | BIT6 | BIT7;
    TACCR0 = 5000;
}

/*
 * Function to Set Up the Transmitter Mode
 */
void Setup_tx(){
    P1DIR |= TRIG_PIN;
    P1DIR &= ~ECHO_PIN;
    TACCR0 = 1000;
   }

/*
 * Function to Get Distance from HC-SR04 Sensor
 */
void getdistance(){
    //Disabling Interupt
     P1IE &= ~0x01;
     //Generating Pulse from Trigger
     P1OUT |= TRIG_PIN;
     __delay_cycles(10);
     //Stopping Pulse from Trigger
     P1OUT &= ~TRIG_PIN;
     //Clearing Flag
     P1IFG = 0x00;
     //Enabling Interrupt for ECHO Pin
     P1IE |= ECHO_PIN;
     //Setting ECHO PIN to Rising Edge
     P1IES &= ~ECHO_PIN;
}

/*
 * Function to Program the Tx End
 */
void Program_tx(){
    int a,b;
    int dist_array[13];

    while(flag < 13){
      getdistance();

      milisec=0;
      while(milisec<=15);

      //Converting ECHO value to CM
      dist_array[flag]=sensorVal/58;
      flag++;
      }
    //Filter to Find the Right Distance from an Array of 13 Element
    for (a = 0; a < 11; a++) {
      for (b = 0; b < 10 - a ; b++) {
         if (dist_array[b] > dist_array[b + 1]) {
             int temp = dist_array[b];
             dist_array[b] = dist_array[b + 1];
             dist_array[b + 1] = temp;
         }
       }
    }
    flag = 0;
    //Transmitting the Distance Value
    uartTransmitData(dist_array[5]);
}

/*
 * Function to Program the Rx End
 */
void Program_rx(){
    buffer(uartReceiveData());
}

// Function to transmit data over UART
void uartTransmitData(int i){
 //Check if USCI_A0 TX buffer is ready
    while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = i;
    }

// Function to receive data over UART
int uartReceiveData(){
 //Check if USCI_A0 RX has been received
    while (!(IFG2 & UCA0RXIFG));
    return UCA0RXBUF;
    }

/*
 * Initializing Timer A0
 */
void initTimer_A0(void){
    TACCR0 = 0;
    TACCTL0 |=CCIE;
    TACTL = TASSEL_2 + ID_0 + MC_1;
}


/*
 * Timer A0 ISR
 * Timer A0 for Creating a Equivalent Display between the Digits
 * Displayed on the Quad Digit Display
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    if(P1IN & BIT0){
    k=(k+1)%3;
    P1OUT=pos[k];
    P2OUT=digit[k];
    }
    else milisec++;
}

/*
 * Defining Port ISR for the ECHO PIN
 * Collecting Time Values at the Rising & Falling Edge to
 * Calculate ECHO Length
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    //Check Interrupt Status
    if(P1IFG & ECHO_PIN){
        //Checking for Rising Edge
        if(!(P1IES & ECHO_PIN)){
            //Clearing Timer A0
            TACTL |= TACLR;
            milisec = 0;
            //Setting to Falling Edge
            P1IES |= ECHO_PIN;
        }
        else{
            //Calculating the ECHO Length
            sensorVal = milisec*1000 + TAR;
        }
        //Clearing Flag
        P1IFG &= ~ECHO_PIN;
    }
}
