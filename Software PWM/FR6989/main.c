//***************************************************************************************
// LAB 4: Timers and PWM
//
// MSP430FR6989 Software PWM
//
//  Kyle Limbaga
//  Rowan University
//  Date Created: October 10, 2019
//  Date Updated: October 14, 2019
//******************************************************************************

#include <msp430.h> 
int Duty_Cycle = 500;
int debounce_state = 0;
void LEDSetup();
void ButtonSetup();
void TimerA0Setup();
void TimerA1Setup();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop Watchdog timer
    LEDSetup();
    ButtonSetup();
    TimerA0Setup();
    TimerA1Setup();

    PM5CTL0 &= ~LOCKLPM5;    // Disable the GPIO power-on default high-impedance mode
                                      // to activate previously configured port settings
   __enable_interrupt();
    for(;;){
        if((TA0R <= Duty_Cycle) && (Duty_Cycle != 0)){
            P9OUT |= BIT7;  //LED P1.0 ON
        }
        else if(TA0R > Duty_Cycle){
            P9OUT &= ~BIT7; //LED P1.0 OFF
        }
        if(!(P1IN & BIT1)){ //button pressed
            P1OUT |= BIT0; //LED 9.7 ON
        }
        else {P1OUT &= ~BIT0;} //LED 1.6 OFF
    }
}
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void){
            switch(debounce_state)
                {
                case 0: //OFF -> GOING ON
                    TA1CTL = TASSEL_2 + MC_1 + TACLR; // SMCLK (1mhz), continuous mode, clear timer
                    P1IE &= ~BIT1; //disable interrupts for P1.1
                    P1IFG &= ~BIT1; //Clear flag P1.1
                    break;
                case 1: //ON -> GOING OFF
                    TA1CTL = TASSEL_2 + MC_1 + TACLR; // SMCLK (1mhz), continuous mode, clear timer
                    P1IE &= ~BIT1; //disable interrupts for P1.1
                    P1IFG &= ~BIT1;  //Clear flag p1.1
                    break;
                }
}
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_ISR(void){
    switch(debounce_state)
    {
    case 0://turning on, on
        if(Duty_Cycle < 1000) //Conditions for Incrementing PWM
        {
            Duty_Cycle += 100;
        }
        else
        {
            Duty_Cycle = 0;
        }
        P1IE |= BIT1; //Enable Interrupts for P1.1
        P1IES &= ~BIT3; //low to high interrupt edge
        TA1CTL &= ~TASSEL_2 + TACLR; //Stop TimerA1, clear timer
        debounce_state = 1;//go to port 1 state 1
        break;
    case 1://turning off, off
        P1IE |= BIT1; //Enable Interrupts for P1.1
        P1IFG &= ~BIT1; //Clear interrupt flag P1.1
        P1IES |= BIT1;//high to low interrupt edge
        TA1CTL &= ~TASSEL_2 + TACLR; //Stop TimerA1, clear timer
        debounce_state = 0; //go to port 2 state 0
        break;
    }
}
void LEDSetup(){
    P1DIR |= BIT0; //Set LED P1.0 to output
    P9DIR |= BIT7; //Set LED P9.7 to output
    P1OUT &= ~BIT0;//turn LED P1.0 off
    P9OUT &= ~BIT7;//turn LED P9.7 off
}
void ButtonSetup(){
    P1DIR &= ~BIT1;//Set P1.1 to input
    P1REN |= BIT1; //Enable Pull Up Resistor for P1.1
    P1OUT |= BIT1; //Active High Button P1.1
    P1IE |= BIT1;  //Enable Interrupt P1.1
    P1IES |= BIT1; //P1.1 Interrupt positive edge trigger
    P1IFG &= ~BIT1; //Clear P1.1 Interrupt Flag
}
void TimerA0Setup(){
    TA0CCR0 = 1000;//Set period for Up mode - for PWM
    TA0CTL |= TASSEL_2 + MC_1 + TACLR; //Use SMCLK (1 Mhz), Up Mode, Clear Timer Registers - for PWM
}
void TimerA1Setup(){
    TA1CCTL0 = CCIE; //interrupt enabled for debouncing
    TA1CCR0 = 50000; //overflow time = 10ms for debouncing
}
