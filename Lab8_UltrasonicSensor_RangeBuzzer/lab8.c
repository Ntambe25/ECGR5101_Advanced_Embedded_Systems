/*
 * Nahush Dilip Tambe & Bharath Kumar Duraisamy Krishnamoorthi
 * ECGR 5101 - Advanced Embedded Systems - Lab # 8
 *
 * For Lab 8, MSP430 board was connected with a SparkFun Quad-Digit Display via UART.
 * An Ultrasonic Sensor, connected to the board measured the distance of an object
 * and sent it to the Display.
 *
 * A buzzer, also connected with the same board alerted at varying frequencies
 * based on the calculated Distance.
 */

#include <msp430.h>
#include <stdint.h>

// Defining Macros for a Clear Code Understanding
#define TRIG_PIN BIT1 // TRIG pin of HC-SR04 connected to P2.1
#define ECHO_PIN BIT0 // ECHO pin of HC-SR04 connected to P2.0
#define BUZZER_PIN BIT4 // Buzzer pin connected to P2.4

// Initializing Global Variables
int flag=0,i,milisec, distance, sensorVal,k=0;
int digit[3];
int dist_array[7];

// Defining Prototype Functions
void initSystem();      // Initializing System
void UART_Setup();      // Initializing UART
void buffer(int n);     // Buffer Function to Store Individual Digits
void PWM_Setup();       // Setting PWM
void Buzzer_Setup();    // Setting Buzzer Pins
void Setup_tx();        // Setting Transmitter

void getdistance();
void Program_tx();      // Programming Transmitter's Functionality
int sorting();
void sendData(char data);

void initTimer_A0(void);


// Start Main Function
int main(void){

  // Initializing System, UART, Timer, PWM, and Buzzer
  initSystem();
  UART_Setup();
  initTimer_A0();
  Buzzer_Setup();
  PWM_Setup();

  __enable_interrupt(); // Enabling Global Interrupts

  Setup_tx();

  // A forever Running While Loop
  while(1){
      Program_tx();
      }
}
/*
 * Function to Initialize System Variables, Select,
 * and Setting Clock Sources
 */
void initSystem(){
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
}

/*
 * Function to set up UART Communication
 */
void UART_Setup(){
    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;
}

/*
 * Breaking Integer Values into Individual Digits
 */
void buffer(int n){
    digit[0]=n%10;
    if(n < 10) digit[1] = 0x78;
    else digit[1] =(n/10)%10;
    if(n < 100) digit[2] = 0x78;
    else digit[2] = (n/100)%10;
    }

/*
 * Function to Set PWM and Timer1
 */
void PWM_Setup(){
    TA1CCR0 = 1000 - 1;
    TA1CCTL2 = OUTMOD_7;
    TA1CCR2 = 1;
    TA1CTL = TASSEL_2 + MC_1;
}

/*
 * Function to Set Buzzer Connection
 */
void Buzzer_Setup() {
    P2DIR |= BUZZER_PIN; // Set BUZZER_PIN (2.4) as output
    P2SEL |= BUZZER_PIN;
}

/*
 * Function to Set up Transmitter Mode
 */
void Setup_tx(){
    P2DIR |= TRIG_PIN;
    P2DIR &= ~ECHO_PIN;
    TACCR0 = 100;
   }

/*
 * Function to get distance from HC-SR04 sensor
 */
void getdistance(){
     P2IE &= ~0x01;       //Disabling Interrupt
     P2OUT |= TRIG_PIN;   //Generating Pulse from Trigger
     milisec=0;
     while(milisec<=1);
     P2OUT &= ~TRIG_PIN;  //Stopping Pulse from Trigger after 10 us
     P2IFG = 0x00;        //Clearing Flag
     P2IE |= ECHO_PIN;    //Enabling Interrupt for ECHO pin
     P2IES &= ~ECHO_PIN;  //Setting ECHO PIN to Rising Edge
}

/*
 * Function to Sort the Distance from the Array
 * and Assigning values to Frequencies for Buzzer
 */
void Program_tx(){
    while(flag<7){
    getdistance();
    milisec=0;
    while(milisec<=300);

    //Converting ECHO value to CM
    distance = sensorVal/58;
    dist_array[flag]=distance;
    flag++;
    }
    flag=0;
    i =sorting();
    // Buzzer will alert only upto a Distance of 50 cm
    if(i<=51){
        // Assigning values to TA1CCR2 for Buzzer
        TA1CCR2 = 1650 - i*30;
    }
    else TA1CCR2 = 0;
    buffer(i);
    sendData(0X76);
    sendData(0X78);
    sendData(digit[2]);
    sendData(digit[1]);
    sendData(digit[0]);
}

/*
 * Filter Function to find the right distance by taking median of 7 values
 * */
int sorting(){
    int a,b;
    for (a = 0; a < 7; a++) {
        for (b = 0; b < 6 - a ; b++) {
           if (dist_array[b] > dist_array[b + 1]) {
              int temp = dist_array[b];
              dist_array[b] = dist_array[b + 1];
              dist_array[b + 1] = temp;
           }
        }
    }
    return dist_array[3];
}
/*
 * Function to Initialize Timer A0
 */
void initTimer_A0(void){
    TACCR0 = 0;
    TACCTL0 |=CCIE;
    TACTL = TASSEL_2 + ID_0 + MC_1;
}

/*
 * Timer A0 ISR to keep track of the Time (milisec variable)
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    milisec++;
}

void sendData(char data) {
        while (!(IFG2 & UCA0TXIFG)); // Wait for the transmit buffer to be ready
        UCA0TXBUF = data; // Send the data
    }

/*
 * Port 2 Vector ISR for the ECHO Pulse
 */
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
    if(P2IFG & ECHO_PIN){//Check interrupt Status
        if(!(P2IES & ECHO_PIN)){//Check rising edge
            TACTL|=TACLR;//Clear timer A
            milisec = 0;
            P2IES |= ECHO_PIN;//Set to Falling edge
        }
        else{
            sensorVal = milisec*100+TAR;//ECHO length
        }
        P2IFG &= ~ECHO_PIN;//Clear flag
    }
}


