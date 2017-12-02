# Overview

This breakout board is designed for the MSP430FR2311 to be used independently for
conection with breadboards and other external components. It features a 16 pin format
for possible connections to P1.1, P1.0, TEST, RST, DVCC, DVSS, P2.7, P2.6, P1.2, P1.3, 
P1.4, P1.5, TRI0-, P1.6, P1.7, and P2.0.

## PCB Board

This boasts a small form factor of less than 1 square inch, measuring in at 
approximately 0.912 inches x 0.906 inches. Its thickness is 0.039 inches. Traces
are 0.006 inches which are compliant with the minimum allowable size. It is a dual 
layer design that contains a ground plane for optimal electromagnetic shielding.

The board was verfied based on design rules, net connectivity, and schematic accuracy.

## Schematic

This design features bypass and bulk capacitors connected between GND and VCC. 
The bulk capacitor ensures a steady voltage provided to the processor, while
bypass capacitors will send low level AC noise to ground, and are typically 
connected proportionally to the number of output pins.

Reset circuitry is included consisting of a capacitor going to ground
to ensure debouncing and a 47k resistor used as a pull up resistor.
 
## Bill of Materials

| Component         | Value         | Quantity      | RefID         |
| ----------------- | ------------- | ------------- | ------------- |
| CAP_0805          | 1 nF	        | x1            | C3            |
| CAP_0603          | 100 nF        | x1            | C1            |
| CAP_0603          | 10 uF         | x1            | C2            |
| RES_0805          | 47k	        | x1            | R1            |
| MSP4302311IPW16R  | N/A           | x1            | U1            |
| 644456-8          | N/A	        | x2            | J1, U2        |