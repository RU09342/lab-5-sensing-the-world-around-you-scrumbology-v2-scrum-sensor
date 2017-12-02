#include <msp430g2553.h>

char LSB = 0;       // UART least significant bit character
char MSB = 0;       // UART most significant bit character
int TX_Data = 0;	// This will hold the read value

int main(void)
{
    
// Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;
    
// Enables interrupts
    __bis_SR_register(GIE);

// Timer Config
    ADC10CTL1 = INCH_7 + SHS_1;         // P1.7, timer A1 triggers sample start
    ADC10AE0 = 0x80;                    // Enable analog input on P1.7
    P1DIR |= 0x01;                      // Set P1.0 to output
    TACCTL0 = CCIE;                     // Enable timer interrupt
    TACCR0 = 256-1;                     // PWM Period
    TACCTL1 = OUTMOD_3;                 // TACCR1 set/reset
    TACCR1 = 128;                       // TACCR1 PWM Duty Cycle 50%
    TACTL = TASSEL_1 + MC_1 + ID_3;     // ACLK as source clock, up mode, /8

// UART Config
    DCOCTL = 0;                         // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;              // Set DCO
    DCOCTL = CALDCO_1MHZ;
    P1SEL |= BIT1 + BIT2 + BIT5;        // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;              // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;               // SMCLK
    UCA0BR0 = 104;                      // 1MHz 9600
    UCA0BR1 = 0;                        // 1MHz 9600
    UCA0MCTL = UCBRS0;                  // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;               // Initialize USCI state machine
    
    while(1){
        // Do nothing
    }
}

// ADC10 interrupt (does conversion A -> D)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)

{
  ADC10CTL0 &= ~ENC;                    // Resets ADC10 conversion
  ADC10CTL0 = 0;                        // Disables Vref
  TX_Data = ADC10MEM;                   // Transfers 10 bit ADC memory to data var
  
  MSB = TX_Data >> 8;                   // Shifts 10 bit int to the right by 8
  LSB = TX_Data;                        // TX_Data is truncated for LSB
  UCA0TXBUF = MSB;                      // Transfers the MSB first
  while(!(UCA0TXIFG));                  // Waits for the TX buffer to be cleared
  UCA0TXBUF = LSB;                      // Transfers the LSB second
}

// Timer A0 interrupt (signals conversion to happen)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)

{
  // Vref = 1.5 V, enable internal Vref
  // Sample and hold for 16 cycles
  // ADC on, enable Interrupts
  ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;
  ADC10CTL0 |= ENC;         // Enable ADC10 conversion

}