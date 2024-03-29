//***************************************************************************************
// LAB 4: Timers and PWM
//
// MSP430G2553 Software PWM
//
//  Kyle Limbaga
//  Rowan University
//  Date Created: October 10, 2019
//  Date Updated: October 14, 2019
//******************************************************************************

#include <msp430.h> 
int Duty_Cycle = 500;
int debounce_state = 0;

    P1DIR |= BIT0; //Set LED P1.0 to output
    P1DIR |= BIT6; //Set LED P1.6 to output
    P1OUT &= ~BIT0;//turn LED P1.0 off
    P1OUT &= ~BIT6;//turn LED P1.6 off

    P1DIR &= ~BIT3;//Set P1.3 to input
    P1REN |= BIT3; //Enable Pull Up Resistor for P1.3
    P1OUT |= BIT3; //Active High Button P1.3
    P1IE |= BIT3;  //Enable Interrupt P1.3
    P1IES |= BIT3; //P1.3 Interrupt positive edge trigger
    P1IFG &= ~BIT3; //Clear P1.3 Interrupt Flag

    TA0CCR0 = 1000;//Set period for Up mode - for PWM
    TA0CTL |= TASSEL_2 + MC_1 + TACLR; //Use SMCLK (1 Mhz), Up Mode, Clear Timer Registers - for PWM


    TA1CCTL0 = CCIE; //interrupt enabled for debouncing
    TA1CCR0 = 50000; //overflow time = 10ms for debouncing

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop Watchdog timer

   __enable_interrupt();
    for(;;){
        if((TA0R <= Duty_Cycle) && (Duty_Cycle != 0)){
            P1OUT |= BIT6;  //LED P1.6 ON
        }
        else if(TA0R > Duty_Cycle){
            P1OUT &= ~BIT6; //LED P1.6 OFF
        }
        if(!(P1IN & BIT3)){ //button pressed
            P1OUT |= BIT0; //LED 1.0 ON
        }
        else {P1OUT &= ~BIT0;} //LED 1.0 OFF
    }
}
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void){
            switch(debounce_state)
                {
                case 0: //OFF -> GOING ON
                    TA1CTL = TASSEL_2 + MC_1 + TACLR; // SMCLK (1mhz), continuous mode, clear timer
                    P1IE &= ~BIT3; //disable interrupts for P1.3
                    P1IFG &= ~BIT3; //Clear flag P1.3
                    break;
                case 1: //ON -> GOING OFF
                    TA1CTL = TASSEL_2 + MC_1 + TACLR; // SMCLK (1mhz), continuous mode, clear timer
                    P1IE &= ~BIT3; //disable interrupts for P1.3
                    P1IFG &= ~BIT3;  //Clear flag p1.3
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
        P1IE |= BIT3; //Enable Interrupts for P1.3
        P1IES &= ~BIT3; //low to high interrupt edge
        TA1CTL &= ~TASSEL_2 + TACLR; //Stop TimerA1, clear timer
        debounce_state = 1;//go to port 1 state 1
        break;
    case 1://turning off, off
        P1IE |= BIT3; //Enable Interrupts for P1.3
        P1IFG &= ~BIT3; //Clear interrupt flag P1.3
        P1IES |= BIT3;//high to low interrupt edge
        TA1CTL &= ~TASSEL_2 + TACLR; //Stop TimerA1, clear timer
        debounce_state = 0; //go to port 2 state 0
        break;
    }
}




