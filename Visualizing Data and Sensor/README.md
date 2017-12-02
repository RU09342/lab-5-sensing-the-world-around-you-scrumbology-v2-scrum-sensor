# Sensors and Visualizing Data

## Overview

The photoresistor, phototransistor, and temperature sensor are devices that sense light 
and produce an analog change in resistance, current, and voltage, respectively. To function 
properly, they each require support circuitry prior to their conversion to digital values. 
The circuit should fullfill the requirements of (1) turning the sensor reading into a voltage, 
(2) filtering noise from the signal, and (3) amplifying it to properly fit the ADC range. 
Lastly, the signal is transmitted over UART at a baud rate of 9600 to be utilized in software.

## ADC10 and ADC12

The ADC core converts an analog input to its 10-bit digital representation and stores 
the result in the ADC10MEM register. The output is full scale (0xFF) when the input 
signal is >= Vref(+) and zero (0x00) when <=  Vref(-). The ADC10 core is configured 
by two control registers, ADC10CTL0 and ADC10CTL1. 

ACLK was chosen as the source clock because ADC10OSC, the default internal oscillator,
is in the 5 MHz range but varies based on voltage, and temperature, and device
characteristics. This is not ideal for sensors that wish to observe a varying temp.

The pin chosen to have the analog reading and input to the ADC is A7 (P1.7). It is
sampled approximately once per second with reference to 1.5V. The code makes use of 
low-power mode while the waiting for an interrupt. Timer_A used in upmode drives the
ADC conversion by triggering it at a set interval. Upon conversion, ADC10_ISR will 
disable ADC10 and Vref and compare ADC10 conversion code.

Finally, the sampled reading is coverted to the desired output and sent over UART.
For example, Vout for a temperature sensor that outputs 10 mV per degree C is divided
by 100 to obtain the degrees C output.

The process for ADC12 is similar on the MSP430FR6989, using a timed interrupt to drive
the conversion rate. In addition to UART, the reading is shown on the LCD display.

## Photoresistor (ADC10) [Resistance Circuit]

### Purpose

As a variable passive component, a photoresister is capable of changing its resistance
according to incoming light. This resistance varies inversely with an increase in brightness.
As such, pure darkness can result in a resistance in the order of Mega-Ohms, while a bright 
light may induce a resistance in the order of 100s of ohms.

### Circuit

The device can be implemented using a simple voltage divider between the photoresistor and 
a 1k resistor. The ADC takes in the voltage between the two resistors and converts it for 
use incalculations.

For maximum sensitivity, it is necessary for the resistane of the companion resistor to be 
equal to the photoresistor's resistance at "typical" room lighting. Experimentally, this 
value was found to be approximately 300 ohms, as the photoresistor ranges from 50-500 ohms. 
The chosen value ensures maximum range of the ADC.

[INSERT CIRCUIT]

### Data

Finally, once the resistance of the photoresister was found the value was transmitted via 
UART and displayed on RealTerm. This allowed the resistance to be easily viewed and analyzed.

## Temperature Sensor (ADC12) [Voltage Circuit]

### Purpose

The temperature sensor is able to vary its output voltage depending on the temperature in 
a room. It varies proportionally; that is, when the temperature is warmer the voltage 
increases, and when temperature cools voltage decreases.

### Circuit

Implementation for the temperature was straightforward. The positiv and negative rails of 
the LM35 were connected to VCC and GND, respectively. The middle pin connects to the ADC.

Because the LM35 outputs 10mV per °C, a non-inverting op-amp was used to amplify the voltage 
by (10k/6.8k + 1 = 2.5), which takes advantage of the full range of the ADC.

[INSERT CIRCUIT]

### Data

The voltage of the temperature sensor was transmitted over UART to be viewed and analyzed.

## Phototransistor (ADC10) [Current Circuit]

### Purpose

A phototransistor is a light-sensitive transistor. It was chosen because it typically
provides much better sensitivity than a photodiode. The structure of the phototransistor 
is specifically optimised for photo applications. As such, it has much larger base and 
collector areas than would be used for a normal transistor. 

### Circuit

Photo transistors are operated in their active regime, although the base connection is left 
open circuit or disconnected because it is not required. It would only be used to bias the 
transistor so that additional collector current was flowing. This would mask any current 
flowing as a result of the photo-action.

The collector is connected high to 3.3 V, while the emitter is connected with a 100k ohm
resistor to grorund. The base is where the voltage is read from.

[INSERT CIRCUIT]

### Data

The voltage of the phototransistor was transmitted over UART to be viewed and analyzed.