
#include <reg52.h>

// LED connections
sbit LED1 = P1^0;
sbit LED2 = P1^1;
sbit LED3 = P1^2;
sbit LED4 = P1^3;

// Push button connected to INT0
sbit BUTTON = P3^2;

// Software time counter
volatile unsigned int ms_ticks = 0;

// Operating mode
unsigned char mode = 0;

// Pattern counter
unsigned char step = 0;

// Timing variables
unsigned int lastPattern = 0;

// Button interrupt and debounce variables
volatile unsigned char buttonEvent = 0;

unsigned char pendingPress = 0;
unsigned char buttonLocked = 0;
unsigned char releaseTiming = 0;

unsigned int pressCheckTime = 0;
unsigned int releaseStart = 0;

#define DEBOUNCE_TIME 30
#define PATTERN_DELAY 250


// --------------------------------
// TIMER 0 INTERRUPT - 1 ms
// --------------------------------

void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x66;

    ms_ticks++;
}


// --------------------------------
// EXTERNAL INTERRUPT 0
// --------------------------------

void INT0_ISR(void) interrupt 0
{
    EX0 = 0;          // Disable INT0 temporarily
    buttonEvent = 1; // Inform main program
}


// --------------------------------
// GET CURRENT TIME
// --------------------------------

unsigned int get_ms(void)
{
    unsigned int t;

    ET0 = 0;

    t = ms_ticks;

    ET0 = 1;

    return t;
}


// --------------------------------
// TIMER 0 INITIALIZATION
// --------------------------------

void Timer0_Init(void)
{
    TMOD = (TMOD & 0xF0) | 0x01;

    TH0 = 0xFC;
    TL0 = 0x66;

    ET0 = 1;

    TR0 = 1;
}


// --------------------------------
// EXTERNAL INTERRUPT INITIALIZATION
// --------------------------------

void INT0_Init(void)
{
    BUTTON = 1;

    IT0 = 1;    // Falling edge triggered

    IE0 = 0;

    EX0 = 1;

    EA = 1;
}


// --------------------------------
// DISPLAY LED PATTERN
// --------------------------------

void LED_Display(unsigned char value)
{
    P1 = (P1 & 0xF0) | (value & 0x0F);
}


// --------------------------------
// MAIN PROGRAM
// --------------------------------

void main(void)
{
    unsigned int now;

    // Initially all LEDs OFF
    P1 = 0xF0;

    BUTTON = 1;

    Timer0_Init();

    INT0_Init();

    while(1)
    {
        now = get_ms();


        // ----------------------------
        // BUTTON PRESS DETECTION
        // ----------------------------

        if(buttonEvent == 1)
        {
            buttonEvent = 0;

            pendingPress = 1;

            buttonLocked = 1;

            releaseTiming = 0;

            pressCheckTime = now;
        }


        // ----------------------------
        // DEBOUNCE BUTTON PRESS
        // ----------------------------

        if(pendingPress == 1)
        {
            if((unsigned int)(now - pressCheckTime) >= DEBOUNCE_TIME)
            {
                pendingPress = 0;

                // Confirm button is still pressed
                if(BUTTON == 0)
                {
                    // Move to next mode
                    mode++;

                    if(mode > 4)
                    {
                        mode = 1;
                    }

                    step = 0;

                    lastPattern = now;

                    // Enter Mode 1
                    if(mode == 1)
                    {
                        LED_Display(0x01);
                    }

                    // Enter Mode 2
                    else if(mode == 2)
                    {
                        LED_Display(0x00);
                    }

                    // Enter Mode 3
                    else if(mode == 3)
                    {
                        LED_Display(0x0A);
                    }

                    // Enter Mode 4
                    else if(mode == 4)
                    {
                        LED_Display(0x0F);
                    }
                }
            }
        }


        // ----------------------------
        // BUTTON RELEASE DETECTION
        // ----------------------------

        if(buttonLocked == 1)
        {
            if(BUTTON == 1)
            {
                if(releaseTiming == 0)
                {
                    releaseTiming = 1;

                    releaseStart = now;
                }

                else if((unsigned int)(now - releaseStart) >= DEBOUNCE_TIME)
                {
                    buttonLocked = 0;

                    releaseTiming = 0;

                    // Enable next button interrupt
                    IE0 = 0;

                    EX0 = 1;
                }
            }
            else
            {
                releaseTiming = 0;
            }
        }


        // ----------------------------
        // TIMER-BASED LED PATTERNS
        // ----------------------------

        if(mode != 0)
        {
            if((unsigned int)(now - lastPattern) >= PATTERN_DELAY)
            {
                lastPattern = now;

                // MODE 1 - RUNNING LED
                if(mode == 1)
                {
                    step++;

                    if(step >= 4)
                    {
                        step = 0;
                    }

                    LED_Display(1 << step);
                }


                // MODE 2 - BINARY COUNTER
                else if(mode == 2)
                {
                    step++;

                    if(step >= 16)
                    {
                        step = 0;
                    }

                    LED_Display(step);
                }


                // MODE 3 - ALTERNATING PATTERN
                else if(mode == 3)
                {
                    step = !step;

                    if(step == 0)
                    {
                        LED_Display(0x0A);
                    }
                    else
                    {
                        LED_Display(0x05);
                    }
                }


                // MODE 4 - ALL LED FLASH
                else if(mode == 4)
                {
                    step = !step;

                    if(step == 0)
                    {
                        LED_Display(0x0F);
                    }
                    else
                    {
                        LED_Display(0x00);
                    }
                }
            }
        }
    }
}
