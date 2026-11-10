#include <reg52.h>

sbit LED = P1^0;

unsigned char pwm_count = 0;
unsigned char duty = 0;
unsigned char fade_count = 0;
unsigned char direction = 1;

void timer0_ISR(void) interrupt 1
{
    TH0 = 0xFF;
    TL0 = 0xF7;

    if(pwm_count < duty)
    {
        LED = 1;
    }
    else
    {
        LED = 0;
    }

    pwm_count++;

    if(pwm_count >= 100)
    {
        pwm_count = 0;
        fade_count++;

        if(fade_count >= 10)
        {
            fade_count = 0;

            if(direction == 1)
            {
                if(duty < 100)
                {
                    duty++;
                }
                else
                {
                    direction = 0;
                }
            }
            else
            {
                if(duty > 0)
                {
                    duty--;
                }
                else
                {
                    direction = 1;
                }
            }
        }
    }
}

void main(void)
{
    LED = 0;

    TMOD = 0x01;

    TH0 = 0xFF;
    TL0 = 0xF7;

    ET0 = 1;
    EA = 1;
    TR0 = 1;

    while(1)
    {
    }
}