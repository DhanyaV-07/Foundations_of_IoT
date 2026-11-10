
#include <reg52.h>

sbit BUTTON = P3^2;
sbit LED = P1^0;

unsigned int ms_ticks = 0;

unsigned char lastRawState = 1;
unsigned char buttonState = 1;

unsigned int debounceTime = 0;

// Timer 0 interrupt - 1 ms
void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x66;

    ms_ticks++;
}

// Read software time safely
unsigned int get_ms(void)
{
    unsigned int t;

    ET0 = 0;
    t = ms_ticks;
    ET0 = 1;

    return t;
}

// Timer 0 initialization
void Timer0_Init(void)
{
    TMOD = (TMOD & 0xF0) | 0x01;

    TH0 = 0xFC;
    TL0 = 0x66;

    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void main(void)
{
    unsigned char currentState;
    unsigned int now;

    BUTTON = 1;
    LED = 0;

    Timer0_Init();

    while(1)
    {
        currentState = BUTTON;
        now = get_ms();

        // Detect raw state change
        if(currentState != lastRawState)
        {
            debounceTime = now;
            lastRawState = currentState;
        }

        // Check whether state is stable for 30 ms
        if((unsigned int)(now - debounceTime) >= 30)
        {
            if(currentState != buttonState)
            {
                buttonState = currentState;

                // Toggle only when button is pressed
                if(buttonState == 0)
                {
                    LED = !LED;
                }
            }
        }
    }
}
