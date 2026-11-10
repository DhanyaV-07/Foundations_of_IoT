
#include <reg51.h>

sbit LED = P1^0;

unsigned int count = 0;

void Timer0_ISR(void) interrupt 1
{
    // Reload Timer 0 for 1 ms
    TH0 = 0xFC;
    TL0 = 0x66;

    count++;

    // 1000 interrupts = 1 second
    if(count >= 1000)
    {
        LED = ~LED;
        count = 0;
    }
}

void main()
{
    // Configure Timer 0 in Mode 1 (16-bit)
    TMOD = 0x01;

    // Load initial timer values
    TH0 = 0xFC;
    TL0 = 0x66;

    // Enable Timer 0 interrupt
    ET0 = 1;

    // Enable global interrupt
    EA = 1;

    // Start Timer 0
    TR0 = 1;

    while(1)
    {
        // Main program does nothing
        // Timer interrupt handles LED toggling
    }
}
