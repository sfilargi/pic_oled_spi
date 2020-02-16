/*
 * File:   main.c
 * Author: stavros
 *
 * Created on February 4, 2020, 1:59 PM
 */

#include <xc.h>

#include "oled.h"

#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (XT (crystal oscillator) from 100 kHz to 4 MHz)
#pragma config RSTOSC = HFINT32  // Power-up default value for COSC bits (EXTOSC operating per FEXTOSC bits)
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

#define _XTAL_FREQ 32000000

#define OLED_RST        LATA2
#define OLED_DC         LATA4
#define OLED_DC_DATA    1
#define OLED_DC_COMMAND 0
#define OLED_CS         LATA5

typedef unsigned char uint8_t;
typedef char int8_t;
typedef int int16_t;
typedef unsigned int uint16_t;
typedef long int int32_t;
typedef unsigned long int uint32_t;

#define RGB(r, g, b) (uint16_t)((uint16_t)(r & 0x1f) | (uint16_t)((g & 0x3f) << 5) | (uint16_t)((b & 0x1f) << 11))
//#define RGB(r, g, b) ((uint16_t)(((uint16_t)(0b00100000) << 8) | 0b00000000))

static const char font6x8[0x60][6] = {
    { 0x00,0x00,0x00,0x00,0x00,0x00 } , /*SPC */
    { 0x00,0x00,0x5F,0x00,0x00,0x00 } , /* !  */
    { 0x04,0x03,0x04,0x03,0x00,0x00 } , /* "  */
    { 0x28,0x7E,0x14,0x3F,0x0A,0x00 } , /* #  */
    { 0x24,0x2A,0x7F,0x2A,0x12,0x00 } , /* $  */
    { 0x23,0x13,0x08,0x64,0x62,0x00 } , /* %  */
    { 0x30,0x4E,0x59,0x26,0x50,0x00 } , /* &  */
    { 0x00,0x00,0x02,0x01,0x00,0x00 } , /* '  */
    { 0x00,0x00,0x1C,0x22,0x41,0x00 } , /* (  */
    { 0x41,0x22,0x1C,0x00,0x00,0x00 } , /* )  */
    { 0x22,0x14,0x08,0x14,0x22,0x00 } , /* *  */
    { 0x08,0x08,0x3E,0x08,0x08,0x00 } , /* +  */
    { 0x50,0x30,0x00,0x00,0x00,0x00 } , /* ,  */
    { 0x08,0x08,0x08,0x08,0x08,0x00 } , /* -  */
    { 0x60,0x60,0x00,0x00,0x00,0x00 } , /* .  */
    { 0x20,0x10,0x08,0x04,0x02,0x00 } , /* /  */
    { 0x3E,0x51,0x49,0x45,0x3E,0x00 } , /* 0  */
    { 0x00,0x42,0x7F,0x40,0x00,0x00 } , /* 1  */
    { 0x62,0x51,0x49,0x49,0x46,0x00 } , /* 2  */
    { 0x22,0x41,0x49,0x49,0x36,0x00 } , /* 3  */
    { 0x18,0x14,0x12,0x7F,0x10,0x00 } , /* 4  */
    { 0x2F,0x45,0x45,0x45,0x39,0x00 } , /* 5  */
    { 0x3E,0x49,0x49,0x49,0x32,0x00 } , /* 6  */
    { 0x01,0x61,0x19,0x05,0x03,0x00 } , /* 7  */
    { 0x36,0x49,0x49,0x49,0x36,0x00 } , /* 8  */
    { 0x26,0x49,0x49,0x49,0x3E,0x00 } , /* 9  */
    { 0x00,0x36,0x36,0x00,0x00,0x00 } , /* :  */
    { 0x00,0x56,0x36,0x00,0x00,0x00 } , /* ;  */
    { 0x00,0x08,0x14,0x22,0x41,0x00 } , /* <  */
    { 0x14,0x14,0x14,0x14,0x14,0x00 } , /* =  */
    { 0x41,0x22,0x14,0x08,0x00,0x00 } , /* >  */
    { 0x02,0x01,0x59,0x09,0x06,0x00 } , /* ?  */
    { 0x3E,0x41,0x5D,0x55,0x2E,0x00 } , /* @  */
    { 0x60,0x1C,0x13,0x1C,0x60,0x00 } , /* A  */
    { 0x7F,0x49,0x49,0x49,0x36,0x00 } , /* B  */
    { 0x3E,0x41,0x41,0x41,0x22,0x00 } , /* C  */
    { 0x7F,0x41,0x41,0x22,0x1C,0x00 } , /* D  */
    { 0x7F,0x49,0x49,0x49,0x41,0x00 } , /* E  */
    { 0x7F,0x09,0x09,0x09,0x01,0x00 } , /* F  */
    { 0x1C,0x22,0x41,0x49,0x3A,0x00 } , /* G  */
    { 0x7F,0x08,0x08,0x08,0x7F,0x00 } , /* H  */
    { 0x00,0x41,0x7F,0x41,0x00,0x00 } , /* I  */
    { 0x20,0x40,0x40,0x40,0x3F,0x00 } , /* J  */
    { 0x7F,0x08,0x14,0x22,0x41,0x00 } , /* K  */
    { 0x7F,0x40,0x40,0x40,0x00,0x00 } , /* L  */
    { 0x7F,0x04,0x18,0x04,0x7F,0x00 } , /* M  */
    { 0x7F,0x04,0x08,0x10,0x7F,0x00 } , /* N  */
    { 0x3E,0x41,0x41,0x41,0x3E,0x00 } , /* O  */
    { 0x7F,0x09,0x09,0x09,0x06,0x00 } , /* P  */
    { 0x3E,0x41,0x51,0x21,0x5E,0x00 } , /* Q  */
    { 0x7F,0x09,0x19,0x29,0x46,0x00 } , /* R  */
    { 0x26,0x49,0x49,0x49,0x32,0x00 } , /* S  */
    { 0x01,0x01,0x7F,0x01,0x01,0x00 } , /* T  */
    { 0x3F,0x40,0x40,0x40,0x3F,0x00 } , /* U  */
    { 0x03,0x1C,0x60,0x1C,0x03,0x00 } , /* V  */
    { 0x0F,0x70,0x0F,0x70,0x0F,0x00 } , /* W  */
    { 0x41,0x36,0x08,0x36,0x41,0x00 } , /* X  */
    { 0x01,0x06,0x78,0x02,0x01,0x00 } , /* Y  */
    { 0x61,0x51,0x49,0x45,0x43,0x00 } , /* Z  */
    { 0x00,0x00,0x7F,0x41,0x41,0x00 } , /* [  */
    { 0x15,0x16,0x7C,0x16,0x11,0x00 } , /* \  */
    { 0x41,0x41,0x7F,0x00,0x00,0x00 } , /* ]  */
    { 0x00,0x02,0x01,0x02,0x00,0x00 } , /* ^  */
    { 0x40,0x40,0x40,0x40,0x40,0x00 } , /* _  */
    { 0x00,0x01,0x02,0x00,0x00,0x00 } , /* `  */
    { 0x00,0x20,0x54,0x54,0x78,0x00 } , /* a  */
    { 0x00,0x7F,0x44,0x44,0x38,0x00 } , /* b  */
    { 0x00,0x38,0x44,0x44,0x28,0x00 } , /* c  */
    { 0x00,0x38,0x44,0x44,0x7F,0x00 } , /* d  */
    { 0x00,0x38,0x54,0x54,0x18,0x00 } , /* e  */
    { 0x00,0x04,0x3E,0x05,0x01,0x00 } , /* f  */
    { 0x00,0x08,0x54,0x54,0x3C,0x00 } , /* g  */
    { 0x00,0x7F,0x04,0x04,0x78,0x00 } , /* h  */
    { 0x00,0x00,0x7D,0x00,0x00,0x00 } , /* i  */
    { 0x00,0x40,0x40,0x3D,0x00,0x00 } , /* j  */
    { 0x00,0x7F,0x10,0x28,0x44,0x00 } , /* k  */
    { 0x00,0x01,0x7F,0x00,0x00,0x00 } , /* l  */
    { 0x7C,0x04,0x7C,0x04,0x78,0x00 } , /* m  */
    { 0x00,0x7C,0x04,0x04,0x78,0x00 } , /* n  */
    { 0x00,0x38,0x44,0x44,0x38,0x00 } , /* o  */
    { 0x00,0x7C,0x14,0x14,0x08,0x00 } , /* p  */
    { 0x00,0x08,0x14,0x14,0x7C,0x00 } , /* q  */
    { 0x00,0x7C,0x08,0x04,0x04,0x00 } , /* r  */
    { 0x00,0x48,0x54,0x54,0x24,0x00 } , /* s  */
    { 0x00,0x04,0x3E,0x44,0x40,0x00 } , /* t  */
    { 0x00,0x3C,0x40,0x40,0x7C,0x00 } , /* u  */
    { 0x00,0x7C,0x20,0x10,0x0C,0x00 } , /* v  */
    { 0x1C,0x60,0x1C,0x60,0x1C,0x00 } , /* w  */
    { 0x00,0x6C,0x10,0x10,0x6C,0x00 } , /* x  */
    { 0x00,0x4C,0x50,0x30,0x1C,0x00 } , /* y  */
    { 0x00,0x44,0x64,0x54,0x4C,0x00 } , /* z  */
    { 0x00,0x08,0x36,0x41,0x41,0x00 } , /* {  */
    { 0x00,0x00,0x7F,0x00,0x00,0x00 } , /* |  */
    { 0x41,0x41,0x36,0x08,0x00,0x00 } , /* }  */
    { 0x08,0x04,0x08,0x10,0x08,0x00 } , /* ~  */
    { 0x00,0x00,0x00,0x00,0x00,0x00 }    /*null*/
};

int g_x = 0;
int g_y = 0;

void
spi_write (char c)
{
    SSP1BUF;
    SSP1BUF = c;
    while(!SSP1STATbits.BF);
}

void
oled_cmd (char c)
{
    OLED_DC = OLED_DC_COMMAND;
    OLED_CS = 0;
    spi_write(c);
    OLED_CS = 1;
}

void
oled_data (char c)
{
    OLED_DC = OLED_DC_DATA;
    OLED_CS = 0;
    spi_write(c);
    OLED_CS = 1;
}

void
oled_pixel (uint8_t x, uint8_t y, uint8_t dx, uint8_t dy, uint16_t color)
{
    oled_cmd(OLED_CMD_SETCOLUMN);
    oled_cmd(x);
    oled_cmd(x + dx - 1);
    oled_cmd(OLED_CMD_SETROW);
    oled_cmd(y);
    oled_cmd(y + dy - 1);
    for (uint16_t count = (dx) * (dy); count > 0; count--) {
        
        oled_data((char)((color >> 8) & 0xFF));
        oled_data((char)(color & 0xFF));
    }
}

void
oled_clear (void)
{
    oled_pixel(0, 0, 96, 64, 0);
}

void
oled_putc (char c, uint16_t color)
{
    if (c == '\n') {
        g_x = 0;
        g_y++;
    } else if (c == ' ') {
        g_x++;
    } else {
        for (uint8_t dx = 0; dx < 6; dx++) {
            char x = g_x * 6 + dx;
            char y = g_y * 8;
            char pixels = font6x8[c - 32][dx];
            for (uint8_t dy = 0; dy < 8; dy++) {
                oled_pixel(x, y + dy, 1, 1,
                        ((pixels >> dy) & 0x1) == 0x1 ? color : 0x0);
            }
        }
        g_x++;
    }
    if (g_x >= 16) {
        g_x = 0;
        g_y++;
    }
    if (g_y >= 8) {
        g_y = 0;
    }
}

void
oled_puts_c (char *s, uint16_t color)
{
    char c;
    while ((c = *s++) != 0)
        oled_putc(c, color);
}

#define oled_puts(str) oled_puts_c(str, 0xFFFF)

void
oled_init (void)
{
      // reset pin
    OLED_RST = 0;
    __delay_ms(1);
    OLED_RST = 1;
        
    oled_cmd(OLED_CMD_DISPLAYOFF);
    oled_cmd(OLED_CMD_SETREMAP);
    //oled_cmd(0x72); // RGB		
    oled_cmd(0x76); // BGR
    oled_cmd(OLED_CMD_STARTLINE);
    oled_cmd(0x0);
    oled_cmd(OLED_CMD_DISPLAYOFFSET);
    oled_cmd(0x0);
    oled_cmd(OLED_CMD_NORMALDISPLAY);
    oled_cmd(OLED_CMD_SETMULTIPLEX);
    oled_cmd(0x3F);
    oled_cmd(OLED_CMD_SETMASTER);
    oled_cmd(0x8E);
    oled_cmd(OLED_CMD_POWERMODE);
    oled_cmd(0x0B);
    oled_cmd(OLED_CMD_PRECHARGE);
    oled_cmd(0x31);
    oled_cmd(OLED_CMD_CLOCKDIV);
    oled_cmd(0xF0);
    oled_cmd(OLED_CMD_PRECHARGEA);
    oled_cmd(0x64);
    oled_cmd(OLED_CMD_PRECHARGEB);
    oled_cmd(0x78);
    oled_cmd(OLED_CMD_PRECHARGEC);
    oled_cmd(0x64);
    oled_cmd(OLED_CMD_PRECHARGELEVEL);
    oled_cmd(0x3A);
    oled_cmd(OLED_CMD_VCOMH);
    oled_cmd(0x3E);
    oled_cmd(OLED_CMD_MASTERCURRENT);
    oled_cmd(0x06);
    oled_cmd(OLED_CMD_CONTRASTA);
    oled_cmd(0x91);
    oled_cmd(OLED_CMD_CONTRASTB);
    oled_cmd(0x50);
    oled_cmd(OLED_CMD_CONTRASTC);
    oled_cmd(0x7D);
    oled_cmd(OLED_CMD_DISPLAYON);
}

void main (void)
{
    OSCFRQbits.HFFRQ = 0b0110; // 32Mhz
    OSCCON1bits.NOSC = 0b000;  // HFINTOSC with 2x PLL
    OSCCON1bits.NDIV = 0b0000; 
    
    while (OSCCON3bits.ORDY != 1); // Wait until clock is selected

    TRISA0 = 0; // output, SCLK
    TRISA1 = 0; // output, MOSI -> SDA
    TRISA2 = 0; // output, Reset
    TRISA4 = 0; // output, Data/Command pin
    TRISA5 = 0; // output, CS/Chip Select/SS in SPI pin
    
    // reset pin
    OLED_RST = 0;
    OLED_CS = 0;
    LATA0 = 0;
    __delay_ms(1);
    OLED_RST = 1;
    
    // Unlock PPS
    //GIE = 0;
    //PPSLOCK = 0x55;
    //PPSLOCK = 0xAA;
    //PPSLOCK = 0x00; // unlock PPS
    
    RA0PPSbits.RA0PPS = 0b11000; // RA0 is clock
    RA1PPSbits.RA1PPS = 0b11001; // RA1 is MOSI
        
    // Lock PPS
    // PPSLOCK = 0x55;
    // PPSLOCK = 0xAA;
    // PPSLOCK = 0x01; // lock   PPS
    // GIE = 0; // still disabled
    
    // Setup SPI
    SSP1STATbits.SMP = 0; // Input data is sampled at the middle of data output time
    SSP1STATbits.CKE = 1; // Transmit occurs on transition from Idle to active clock state
    SSP1CON1bits.CKP = 0; // Idle state for clock is a low level
    SSP1CON1bits.SSPM = 0b0010; // SPI Master mode, clock = FOSC/64
    SSP1CON1bits.SSPEN = 0b1; // Enables serial port and configures SCK, SDO, SDI and SS as the source of the serial port pin
    
    SSP1CON3bits.PCIE = 0; // Stop detection interrupts are disabled
    SSP1CON3bits.SCIE = 0; // Start detection interrupts are disabled
    
    oled_init();
    oled_clear();

    //oled_puts("01234567890123451\n2\n3\n4\n5\n6\n7");
    oled_puts("1: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("2: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("3: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("4: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("5: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("6: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("7: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    oled_puts("8: ");
    oled_puts_c("1200\n",  RGB(31, 2, 1));
    
    while(1) {
    }
}
