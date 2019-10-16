//***************************************************************************************
// LAB 4: Timers and PWM
//
// MSP430FR6989 Debounce
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

    P1DIR |= BIT0;  //sets LED 1.0 as output
    P1OUT &= ~BIT0; //sets LED on P1.0 off initially

    P1DIR &= ~BIT1; //set P1.1 as input
    P1REN |= BIT1;  //enables pull up or pull down for P1.1
    P1OUT |= BIT1;  //enables pull up resistor for P1.1
    P1IE |= BIT1;   //enable button interrupt for P1.1
    P1IFG &= ~BIT1; //clear flag for button interrupt for P1.1
    P1IES |= BIT1;  //P1.1 button interrupt on positive edge

    TA0CCTL0 = CCIE;    //CCR0 interrupt enabled
    TA0CCR0 = 50000;    //overflow every 10ms

    PM5CTL0 &= ~LOCKLPM5;    // Disable the GPIO power-on default high-impedance mode
                                    // to activate previously configured port settings

    __enable_interrupt();

}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
        switch(debounce_state)
            {
            case 0: //LED turning on
                TA0CTL = TASSEL_2 + MC_1 + TACLR;       //SMCLK in continuous mode
                P1IE &= ~BIT1;                          //P1.1 button interrupt disabled
                P1IFG &= ~BIT1;
                break;
            case 1: //LED turning off
                TA0CTL = TASSEL_2 + MC_1 + TACLR;       //SMCLK in continuous mode
                P1IE &= ~BIT1;                          //P1.1 button interrupt disabled
                P1IFG &= ~BIT1;
                break;
            }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
    switch(debounce_state)
    {
    case 0://LED turning on
        P1OUT ^= BIT0;  //switch state of P1.0 LED
        P1IE |= BIT1;   //P1.1 button interrupt enabled
        P1IES &= ~BIT1; //toggle interrupt edge on button release
        TA0CTL &= ~TASSEL_2;    //stop TimerA0
        TA0CTL |= TACLR;    //clear TimerA0
        debounce_state = 1; //go to next state
        break;
    case 1://LED turning off
        P1IE |= BIT1;   //P1.1 button interrupt enabled
        P1IFG &= ~BIT1; //P1.1 IFG cleared
        P1IES |= BIT1;  //set P1.1 button interrupt to High to Low
        TA0CTL &= ~TASSEL_2;    //stop TimerA0
        TA0CTL |= TACLR;    //clear TimerA0
        debounce_state = 0; //go to case 0 when button is pressed
        break;
    }

}
