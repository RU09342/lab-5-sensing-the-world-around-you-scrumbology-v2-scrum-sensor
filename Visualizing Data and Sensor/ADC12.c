#include <msp430.h> 
#include "LCDDriver.h"
//LAB 5 ADC12
//Tyler Brady
//Accidentally overwrote with lab 6 code, doesn't need to be fixed since you can do the program without changing pwm.


void LCDDisplay(void)
{
    // Initialize LCD_C
        // Initialize LCD segments 0 - 21; 26 - 43
        LCDCPCTL0 = 0xFFFF;
        LCDCPCTL1 = 0xFC3F;
        LCDCPCTL2 = 0x0FFF;
        // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
        LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

        // VLCD generated internally,
        // V2-V4 generated internally, v5 to ground
        // Set VLCD voltage to 2.60v
        // Enable charge pump and select internal reference for it
        LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

        LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled

        LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory

        //Turn LCD on
        LCDCCTL0 |= LCDON;
}

unsigned int CurrentValue = 0;
const unsigned char lcd_num[10] = {
    0xFC,        // 0
    0x60,        // 1
    0xDB,        // 2
    0xF3,        // 3
    0x67,        // 4
    0xB7,        // 5
    0xBF,        // 6
    0xE4,        // 7
    0xFF,        // 8
    0xF7,        // 9
};


void displayLCD(void)
{
    char a, b, c, d;
    int newval;
    int nextval;

    a = (ADC12MEM0/1000);
    newval =  ADC12MEM0 - (a*1000);
    b = (newval/100);
    nextval = newval - (b*100);
    c = (nextval/10);
    d = (nextval - (c*10));

    showChar((a + '0'),1);
    showChar((b + '0'),2);
    showChar((c + '0'),3);
    showChar((d + '0'),4);
}

unsigned int ADCRead(void)
{
    ADC12CTL0 |= ADC12ENC | ADC12SC;        //Start sampling/conversion
    while(!(ADC12IFGR0 & ADC12IFG0));
    return ADC12MEM0;
}

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    // GPIO Setup
    P8SEL1 |= BIT6;                         // Configure P8.6 for ADC
    P8SEL0 |= BIT6;

    P3DIR |= BIT6;                     // PWM P3.6 output
    P3SEL1 |= BIT6;                    //PWM P3.6 options select

    P2SEL0 |= BIT0 | BIT1;                    // Configure UART pins
    P2SEL1 &= ~(BIT0 | BIT1);

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;


    // Startup clock system with max DCO setting ~8MHz
     CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
     CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
     CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
     CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
     CSCTL0_H = 0;                             // Lock CS registers



    LCDDisplay(); //Enable LCD display


    // Configure ADC12
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;      // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                   // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_5;              // A5 ADC input select; Vref=AVCC


    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt


    //PWM
    TB0CCR0 = 255-1;                         // PWM Period
    TB0CCTL2 = OUTMOD_7;                      // CCR2 reset/set
    TB0CCR2 = 255-1;                            // CCR2 PWM duty cycle
    TB0CTL = TBSSEL__SMCLK | MC__UP | ID_3 | TBCLR;  // SMCLK, up mode, clear TBR

    __bis_SR_register(GIE);

    while(1){
        CurrentValue = ADCRead();
        displayLCD();
        if (CurrentValue < 0x1FF)
            P1OUT |= BIT0;
        else
            P1OUT &= ~BIT0;

        /*
        switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
          {
            case USCI_NONE: break;
            case USCI_UART_UCRXIFG:
              TB0CCR2 = UCA0RXBUF;
              __no_operation();
              break;
            case USCI_UART_UCTXIFG: break;
            case USCI_UART_UCSTTIFG: break;
            case USCI_UART_UCTXCPTIFG: break;
          }
*/

        //__delay_cycles(300000);
    }
}


#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
              {
                case USCI_NONE: break;
                case USCI_UART_UCRXIFG:
                  TB0CCR2 = UCA0RXBUF;
                  //__no_operation();
                  break;
                case USCI_UART_UCTXIFG: break;
                case USCI_UART_UCSTTIFG: break;
                case USCI_UART_UCTXCPTIFG: break;
              }
}
