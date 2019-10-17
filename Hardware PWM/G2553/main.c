//***************************************************************************************
// LAB 4: Timers and PWM
//
// MSP430G2553 Hardware PWM
//
//  Kyle Limbaga
//  Rowan University
//  Date Created: October 10, 2019
//  Date Updated: October 14, 2019
//******************************************************************************

#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |= BIT0;  //set P1.0 LED to output
    P1DIR |= BIT6;  //set P1.6 LED to output
    P1SEL |= (BIT6);    //set port 1 for output
    P1OUT &= ~BIT0; //initialize LED to off
    P1DIR &= ~BIT3; //set P1.3 button to input
    P1REN |= BIT3;  //enable resistor on P1.3 button
    P1OUT |= BIT3;  //set P1.3 to pull up resistor
    P1IE |= BIT3;   //enable P1.3 button interrupt
    P1IES |= BIT3;  //low to high interrupt edge
    P1IFG &= ~BIT3; //clear P1.3 interrupt flags
    TA0CTL = TASSEL_2 + MC_1 + ID_1;    //UP mode, divide by 2
    TA0CCR0 = 1000; //set period
    TA0CCR1 = 100;  //set duty cycle
    TA0CCTL1 = OUTMOD_7;    //set timer to set/reset mode
    __enable_interrupt();
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IES & BIT3)
    {
            if (TA0CCR1<1000)   //if statement for duty cycle under 100%
            {
                TA0CCR1+=100;               //increment duty cycle by 10%
            }
            else if (TA0CCR1 >=1000)     //if statement for duty cycle 100% or more
            {
                TA0CCR1 = 0;                //reset duty cycle
            }
    }
           P1IES ^= BIT3;   //interrupt edge
           P1OUT ^= BIT0;   //toggle LED
           P1IFG &= ~BIT3;  //clear P1.3 button interrupt flags
}
