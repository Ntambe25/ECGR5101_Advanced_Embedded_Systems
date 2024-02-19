#include <msp430.h>
#include <stdint.h>

// Defining Macros for a Clear Code Understanding
//1nd USonic
#define TRIG_PIN1 BIT1 // TRIG pin of HC-SR04 connected to P2.1
#define ECHO_PIN1 BIT0 // ECHO pin of HC-SR04 connected to P2.0
//2nd USonic
#define TRIG_PIN2 BIT4 // TRIG pin of HC-SR04 connected to P2.4
#define ECHO_PIN2 BIT3 // ECHO pin of HC-SR04 connected to P2.3
//Buzzer
#define BUZZER_PIN BIT2 // Buzzer pin connected to P2.6

// Initializing Global Variables
int flag=0,i,milisec, distance, sensorVal,k=0,cnt=0;
int digit[3];
int dist_array[7];

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
    TA1CCR1 = 1000 - 1;
    TA1CCTL2 = OUTMOD_7;
    TA1CCR2 = 1;
    TA1CTL = TASSEL_2 + MC_1;
}

/*
 * Function to Set Buzzer Connection
 */
void Buzzer_Setup() {
    P2DIR |= BUZZER_PIN;
    P2SEL |= BUZZER_PIN;
}

/*
 * Function to Set up Transmitter Mode
 */
void Setup_tx() {
    P2DIR |= TRIG_PIN1 | TRIG_PIN2;
    P2DIR &= ~(ECHO_PIN1 | ECHO_PIN2);
    TACCR0 = 100;
}

/*
 * Function to get distance from HC-SR04 sensor
 */
void getdistance1(){
     P2IE &= ~0x01;       //Disabling Interrupt
     P2OUT |= TRIG_PIN1;   //Generating Pulse from Trigger
     milisec=0;
     while(milisec<=1);
     P2OUT &= ~TRIG_PIN1;  //Stopping Pulse from Trigger after 10 us
     P2IFG = 0x00;        //Clearing Flag
     P2IE |= ECHO_PIN1;    //Enabling Interrupt for ECHO pin
     P2IES &= ~ECHO_PIN1;  //Setting ECHO PIN to Rising Edge
}

void getdistance2(){
     P2IE &= ~0x01;       //Disabling Interrupt
     P2OUT |= TRIG_PIN2;   //Generating Pulse from Trigger
     milisec=0;
     while(milisec<=1);
     P2OUT &= ~TRIG_PIN2;  //Stopping Pulse from Trigger after 10 us
     P2IFG = 0x00;        //Clearing Flag
     P2IE |= ECHO_PIN2;    //Enabling Interrupt for ECHO pin
     P2IES &= ~ECHO_PIN2;  //Setting ECHO PIN to Rising Edge
}


/*
 * Function to Sort the Distance from the Array
 * and Assigning values to Frequencies for Buzzer
 */
void Program_tx(){
    for(flag=0;flag<5;flag++){
    getdistance1();
    milisec=0;
    while(milisec<=200);
    //Converting ECHO value to CM
    distance = sensorVal/58;
    dist_array[flag]=distance;
    }
    i =sorting();
    // Buzzer will alert only upto a Distance of 50 cm
    if(i<=50){
        milisec=0;
        TA1CCR1 = 300;
        cnt++;
        while(milisec<=10000);
        // Assigning values to TA1CCR2 for Buzzer
    }
    else TA1CCR1 = 0;

    for(flag=0;flag<5;flag++){
    getdistance2();
    milisec=0;
    while(milisec<=200);
    //Converting ECHO value to CM
    distance = sensorVal/58;
    dist_array[flag]=distance;
    }
    i =sorting();
    // Buzzer will alert only upto a Distance of 50 cm
    if(i<=50){
        milisec=0;
        TA1CCR1 = 600;
        cnt--;
        while(milisec<=10000);
        // Assigning values to TA1CCR2 for Buzzer
    }
    else TA1CCR1 = 0;

    if(cnt<0)cnt=0;
    buffer(cnt);
    sendData(0X76);
    sendData(0X78);
    sendData(digit[2]);
    sendData(digit[1]);
    sendData(digit[0]);
//
//    sendData(1);
//    sendData(2);
//    sendData(3);
//    sendData(4);
}

/*
 * Filter Function to find the right distance by taking median of 7 values
 * */
int sorting(){
    int a,b;
    for (a = 0; a < 5; a++) {
        for (b = 0; b < 4 - a ; b++) {
           if (dist_array[b] > dist_array[b + 1]) {
              int temp = dist_array[b];
              dist_array[b] = dist_array[b + 1];
              dist_array[b + 1] = temp;
           }
        }
    }
    return dist_array[2];
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
    if(P2IFG & ECHO_PIN1){//Check interrupt Status
        if(!(P2IES & ECHO_PIN1)){//Check rising edge
            TACTL|=TACLR;//Clear timer A
            milisec = 0;
            P2IES |= ECHO_PIN1;//Set to Falling edge
        }
        else{
            sensorVal = milisec*100+TAR;//ECHO length
        }
        P2IFG &= ~ECHO_PIN1;//Clear flag
    }
    else if(P2IFG & ECHO_PIN2){//Check interrupt Status
        if(!(P2IES & ECHO_PIN2)){//Check rising edge
            TACTL|=TACLR;//Clear timer A
            milisec = 0;
            P2IES |= ECHO_PIN2;//Set to Falling edge
        }
        else{
            sensorVal = milisec*100+TAR;//ECHO length
        }
        P2IFG &= ~ECHO_PIN2;//Clear flag
        }
}


