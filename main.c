/*
 * File:   main.c
 * Author: stavros
 *
 * Created on February 4, 2020, 1:59 PM
 */


#include <xc.h>

#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (XT (crystal oscillator) from 100 kHz to 4 MHz)
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits (EXTOSC operating per FEXTOSC bits)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled )
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable bits (WDT enabled, SWDTEN is ignored)
#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bits (Brown-out Reset enabled, SBOREN bit ignored)
#pragma config BORV = LOW       // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF      // Debugger enable bit (Background debugger disabled)

// CONFIG3
#pragma config WRT = OFF        // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON        // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)

// CONFIG4
#pragma config CP = OFF         // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF        // Data NVM Memory Code Protection bit (Data NVM code protection disabled)


#define _XTAL_FREQ 1000000

#define OLED_RST LATA2
#define OLED_DC  LATA4
#define OLED_DC_DATA 1
#define OLED_DC_COMMAND 0
#define OLED_CS  LATA5

void
spi_write (char c)
{
    // reverse?
    int i;
    int r = 0;
    for (i = 0; i < 8; i++) 
    { 
        if((c & (1 << i))) 
           r |= 1 << ((8 - 1) - i);   
    }
    //c = r;
    
    SSP1BUF;
    SSP1BUF = c;
    while(!SSP1STATbits.BF);
}

void
spi_command (char c)
{
    OLED_DC = OLED_DC_COMMAND;
    OLED_CS = 0;
    spi_write(c);
    OLED_CS = 1;
}

void
spi_data (char c)
{
    OLED_DC = OLED_DC_DATA;
    OLED_CS = 0;
    spi_write(c);
    OLED_CS = 1;
}

void main (void)
{
//    OSCFRQbits.HFFRQ = 0b0110; // 32Mhz
//    OSCCON1bits.NOSC = 0b000; // HFINTOSC with 2x PLL
//    OSCCON1bits.NDIV = 0b0000; 
    
    while (OSCCON3bits.ORDY != 1); // Wait until clock is selected

    TRISA0 = 0; // output, SCLK
    TRISA1 = 0; // output, MOSI -> SDA
    TRISA2 = 0; // output, Reset
    TRISA4 = 0; // output, D/C pin
    TRISA5 = 0; // output, CS pin
    
    // reset pin
    OLED_RST = 0;
    OLED_CS = 0;
    LATA0 = 0;
    __delay_ms(1);
    OLED_RST = 1;
    
    // Unlock PPS
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCK = 0x00; // unlock PPS
    
    RA0PPSbits.RA0PPS = 0b11000; // RA0 is clock
    RA1PPSbits.RA1PPS = 0b11001; // RA1 is MOSI
        
    // Lock PPS
    // PPSLOCK = 0x55;
    // PPSLOCK = 0xAA;
    // PPSLOCK = 0x01; // lock   PPS
    // GIE = 0; // still disabled
    
    SSP1STATbits.SMP = 0; // Input data is sampled at the middle of data output time
    SSP1STATbits.CKE = 1; // Transmit occurs on transition from Idle to active clock state
    SSP1CON1bits.CKP = 0; // Idle state for clock is a low level
    SSP1CON1bits.SSPM = 0b0010; // SPI Master mode, clock = FOSC/64
    SSP1CON1bits.SSPEN = 0b1; // Enables serial port and configures SCK, SDO, SDI and SS as the source of the serial port pin
    //SSP1CON1bits.SSPEN = 0b0; // Disables serial port and configures these pins as I/O port pins
    SSP1CON3bits.PCIE = 0; // Stop detection interrupts are disabled
    SSP1CON3bits.SCIE = 0; // Start detection interrupts are disabled
    
    // reset pin
    OLED_RST = 0;
    __delay_ms(1);
    OLED_RST = 1;
    spi_command(0xAE);
    spi_command(0xA4);
    spi_command(0xAF);
    spi_command(0xA5);
    
    while(1) {
    }
}
