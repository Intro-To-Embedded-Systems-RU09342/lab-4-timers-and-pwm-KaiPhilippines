//***************************************************************************************
// LAB 4: Timers and PWM
//
// MSP430G2553  Debounce
//
//  Kyle Limbaga
//  Rowan University
//  Date Created: October 10, 2019
//  Date Updated: October 14, 2019
//******************************************************************************

#include <msp430.h>

int debounce_state = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR |= BIT6;  //sets LED 1.6 as output
    P1OUT &= ~BIT6; //sets LED on P1.6 off initially

    P1DIR &= ~BIT3; //set P1.3 as input
    P1REN |= BIT3;  //enables pull up or pull down for P1.3
    P1OUT |= BIT3;  //enables pull up resistor for P1.3
    P1IE |= BIT3;   //enable button interrupt for P1.3
    P1IFG &= ~BIT3; //clear flag for button interrupt for P1.3
    P1IES |= BIT3;  //P1.3 button interrupt on positive edge

    TA0CCTL0 = CCIE;    //CCR0 interrupt enabled
    TA0CCR0 = 50000;    //overflow every 10ms

    __enable_interrupt();

}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
        switch(debounce_state)
            {
            case 0: //LED turning on
                TA0CTL = TASSEL_2 + MC_1 + TACLR;       //SMCLK in continuous mode
                P1IE &= ~BIT3;                          //P1.3 button interrupt disabled
                P1IFG &= ~BIT3;
                break;
            case 1: //LED turning off
                TA0CTL = TASSEL_2 + MC_1 + TACLR;       //SMCLK in continuous mode
                P1IE &= ~BIT3;                          //P1.3 button interrupt disabled
                P1IFG &= ~BIT3;
                break;
            }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
    switch(debounce_state)
    {
    case 0://LED turning on
        P1OUT ^= BIT6;  //switch state of P1.0 LED
        P1IE |= BIT3;   //P1.3 button interrupt enabled
        P1IES &= ~BIT3; //toggle interrupt edge on button release
        TA0CTL &= ~TASSEL_2;    //stop TimerA0
        TA0CTL |= TACLR;    //clear TimerA0
        debounce_state = 1; //go to next state
        break;
    case 1://LED turning off
        P1IE |= BIT3;   //P1.3 button interrupt enabled
        P1IFG &= ~BIT3; //P1.3 IFG cleared
        P1IES |= BIT3;  //set P1.3 button interrupt to High to Low
        TA0CTL &= ~TASSEL_2;    //stop TimerA0
        TA0CTL |= TACLR;    //clear TimerA0
        debounce_state = 0; //go to case 0 when button is pressed
        break;
    }

}
