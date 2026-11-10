
#include <reg52.h>

sbit BUTTON = P3^2;
sbit LED = P1^0;

void main(void)
{
    BUTTON = 1;  // Configure as input

    LED = 0;     // Initially LED OFF

    while(1)
    {
        if(BUTTON == 0)
        {
            LED = 1;   // Button pressed: LED ON
        }
        else
        {
            LED = 0;   // Button released: LED OFF
        }
    }
}
