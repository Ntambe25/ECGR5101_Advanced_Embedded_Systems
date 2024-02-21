/*
 * Nahush Dilip Tambe & Bharath Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 5
 *
 * For Lab 5, Two MSP430g2553 Boards will be Conneected to each other.
 * 1st Board will read a Button Press and Increment a Counter by 1, counting from 0-13.
 * When 13 is reached, next Button Press will reset it to 0.
 *
 * 2nd Microcontroller will receive the Button Press value through UART, convert it to 2^n
 * and display it on a Multiplexed Quad Digit Display.
 */
#include <msp430.h>
#include <stdint.h>           // Using Standard Library for Mathematics Calculations

// Declaring an Array of Hex Values (0x--)
int Display_Array[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8, 0x80, 0x90};
int i=0; // global variable for counter

int main(void){
  WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer
  if(!(P1IN & BIT3)) Setup_tx();
  else Setup_rx();

  DCOCTL = 0; //Clear DCO
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  //P1.1 = RX = BIT1, P1.2 = TX = BIT2
  P1SEL |= BIT1 + BIT2;
  P1SEL2 |= BIT1 + BIT2;
  //Disable USCI, reset mode
  UCA0CTL1 |= UCSWRST;
  //SMCLK
  UCA0CTL1 |= UCSSEL_2;
  //1MHz
  //Baud Rate -> 9600
  UCA0BR0 = 104;
  UCA0BR1 = 0;
  //Modulation UCBRSx = 1
  UCA0MCTL = UCBRS0;
  //Initialize USCI state machine
  UCA0CTL1 &= ~UCSWRST;

  initTimer();
  __enable_interrupt();

   while(1);
}
/*
 * Function to Store the individual digits in a Buffer,
 * then, as needed, pass as arguments to the Display Function
 */
int buffer(int n){
    int digit1=n%10;
    n=(n-digit1)/10;
    int digit2=n%10;
    n=(n-digit2)/10;
    int digit3=n%10;
    n=(n-digit3)/10;
    int digit4=n%10;
    if(digit4==0 && digit3==0 && digit2!=0){
        dig(0x20,digit2);//digit2
    }
    else if(digit3!=0 && digit4==0){
        dig(0x20,digit2);//digit2
        dig(0x40,digit3);//digit3
    }
    else if(digit4==0 && digit3==0 && digit2==0);
    else{
        dig(0x40,digit3);//digit3
        dig(0x80,digit4);//digit4
        dig(0x20,digit2);//digit2
    }
    dig(0x10,digit1);
}
/*
 * Function to Display the Digits on the Multiplexed Quad Display
 */
void dig(int pos,int val){
    P1OUT=pos;
    P2OUT=Display_Array[val];
    __delay_cycles(5000);
}
/*
 * Configuring the Transmitting Board Connection
 * The Button
 */
void Setup_tx(){
    P1SEL |= BIT4;            // Setting P1.0 as Analog Input
    P1DIR &= ~BIT4;
}
/*
 * Configuring the Receiving Board Connections
 * Multiplex Quad Digit Display
 */
void Setup_rx(){
    P2DIR = 0xFF;             // Configuring P2.0 as OUTPUT (7 Segment LED)
    P2SEL = 0x00;             // Setting all Pins on P2 as Outputs
    P1DIR |= BIT4 | BIT5 | BIT6 | BIT7; // Setting P1.4, 5, 6, 7 as Outputs
}
/*
 * Function to Handle The Transmission End
 * If Button Pressed, Increment Counter and Send,
 * the Button Press Value via UART
 */
void Program_tx(){
    if((P1IN & BIT4)==0x00){    /* Check the condition of P1.0 (button)*/
        i=i+1;   /* Increment the counter if P1.0 is low*/
        if(i==14){i=0;}
        __delay_cycles(500000); /* delay for avoiding multiple button presses*/
        }
    uartTransmitData(i);
}
/*
 * Function to Process Received Data via UART
 * When value is received, display 2^value on Quad Digit Display
 */
void Program_rx(){
    int j =uartReceiveData();
    int n=1;
    int k;
    for (k = 0; k < j; k++) {
        n *= 2;
    }
    buffer(n);
    }
/*
 * Function to Transmit Data via UART
 */
void uartTransmitData(int i){
 //Check if USCI_A0 TX buffer is ready
    while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = i;
    }
/*
 * Function to Receive Data via UART Communication
 */
int uartReceiveData(){
 //Check if USCI_A0 RX has been received
    while (!(IFG2 & UCA0RXIFG));
    return UCA0RXBUF;
    }

void initTimer(void) {
    TACCR0 = 30;
    TACTL = TASSEL_2 + MC_1 + ID_3 + TACLR;
    TACCTL0 |= CCIE;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void){
    if(!(P1IN & BIT3)) Program_tx();
    else Program_rx();
    }
