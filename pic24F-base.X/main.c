#include <p24Fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpic30.h>

#define FCY     12000000

// CONFIG3
#pragma config WPFP = WPFP511           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable bit (Segmented code protection disabled)
#pragma config WPCFG = WPCFGDIS         // Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS oscillator mode selected)
#pragma config DISUVREG = OFF           // Internal USB 3.3V Regulator Disable bit (Regulator is disabled)
#pragma config IOL1WAY = ON             // IOLOCK One-Way Set Enable bit (Write RP Registers Once)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSCO functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Oscillator Select (Primary oscillator (XT, HS, EC))
#pragma config PLL_96MHZ = ON           // 96MHz PLL Disable (Enabled)
#pragma config PLLDIV = DIV12           // USB 96 MHz PLL Prescaler Select bits (Oscillator input divided by 12 (48MHz input))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-speed start-up)disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#define LED1 	LATEbits.LATE0
#define LED2	LATEbits.LATE1
#define LED3 	LATEbits.LATE2
#define LED4	LATEbits.LATE3

#define SW1     PORTEbits.RE4
#define SW2     PORTEbits.RE5
#define SW3     PORTEbits.RE6
#define SW4     PORTEbits.RE7

int main(void){

    //*************************************
    // Unlock Registers
    //*************************************
    asm volatile (  "MOV #OSCCON, w1 \n"
                    "MOV #0x46, w2 \n"
                    "MOV #0x57, w3 \n"
                    "MOV.b w2, [w1] \n"
                    "MOV.b w3, [w1] \n"
                    "BCLR OSCCON,#6");
    //***************************
    // Configure Input Functions
    // (See Table 10-2)
    //***************************
    RPINR18bits.U1RXR = 23;   // U1 RX In = RP23
    //***************************
    // Configure Output Functions
    // (See Table 10-3)
    //***************************
    RPOR11bits.RP22R = 3;    // U1 TX Out = RP22
    //*************************************
    // Lock Registers
    //*************************************
    asm volatile (  "MOV #OSCCON, w1 \n"
                    "MOV #0x46, w2 \n"
                    "MOV #0x57, w3 \n"
                    "MOV.b w2, [w1] \n"
                    "MOV.b w3, [w1] \n"
                    "BSET OSCCON, #6" );

    TRISEbits.TRISE0 = 0; 	// Make LED 1 an output
    TRISEbits.TRISE1 = 0;	// Make LED 2 an output
    TRISEbits.TRISE2 = 0;	// Make LED 3 an output
    TRISEbits.TRISE3 = 0;	// Make LED 4 an output

    // Configure UART1
    __C30_UART=1;           // Select UART1 for STDIO
    U1STAbits.OERR = 0;     // Clear any buffer overrun errors
    U1BRG = 38;             // Baud Rate 12MHz/2/(16*9600BPS) - 1
    IPC2bits.U1RXIP = 0x01; // Setup UART1 RX interrupt priority level
    IFS0bits.U1RXIF = 0;    // Clear UART1 RX interrupt flag
    U1MODEbits.UARTEN = 1;	// Enable UART1 peripheral
    IEC0bits.U1RXIE = 1;    // Enable UART1 RX interrupt
    
    // Configure Timer1
    T1CON = 0;              // Stop TMR1 and reset control register
    TMR1 = 0x00;            // Clear contents of timer register
    PR1 = 9375;             // Interrupt every 100mS
    IPC0bits.T1IP = 0x01;   // Setup Timer1 interrupt for desired priority level
    IFS0bits.T1IF = 0;      // Clear Timer1 interrupt status flag
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupts
    T1CONbits.TCKPS = 0b10; // 1:64 pre-scale
    T1CONbits.TCS = 0;      // Timer source: Internal clock (FOSC/2)
    T1CONbits.TON = 1;      // Start Timer1
    
    printf("Hello World\n");
        
    while(1) {

    }
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    LED1 = !LED1;       
    IFS0bits.T1IF = 0;      // Clear Timer1 interrupt flag
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
    unsigned char c;
    c = U1RXREG;            // Read buffer to prevent overrun
    LED2 = !LED2;
    IFS0bits.U1RXIF = 0;    // Clear UART1 receive interrupt flag
}



