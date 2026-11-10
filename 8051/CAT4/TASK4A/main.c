#include <reg52.h> 
 
sbit COL1 = P2^7; 
sbit COL2 = P2^6; 
sbit COL3 = P2^5;
sbit COL4 = P2^4;

sbit LED1 = P1^1;
sbit LED2 = P1^2;
sbit LED3 = P1^3;
sbit LED4 = P1^4;


//Remember current mode 
unsigned char mode = 0;
unsigned char value = 0;

void delay() {
    int i, j;
    for(i = 0; i < 100; i++) {
        for(j = 0; j < 500; j++) {
            
        }
    }
}

void led_control(unsigned char key) {
    P1 = (1 << key);
}

void running_led() {
        P1 = 0x02; 
        delay();
        P1 = 0x04;
        delay();
        P1 = 0x08;
        delay();
        P1 = 0x10;
        delay();
}

void binary_counter() {
    unsigned char count;

    for(count = 0; count < 16; count++)
    {
        LED1 = count & 0x01;
        LED2 = (count >> 1) & 0x01;
        LED3 = (count >> 2) & 0x01;
        LED4 = (count >> 3) & 0x01;

        delay();
    }
}

void alternate_led() {
        P1 = 0x0A;   
        delay();
        P1 = 0x14;   
        delay();
}

void blink_all()
{
    static bit state = 0;

    if(state == 0)
    {
        P1 = 0x1E;   
        state = 1;
    }
    else
    {
        P1 = 0x00;   
        state = 0;
    }

    delay();
}


void display_digit(unsigned char value) {
    if(value == 0) {
        P0 = 0x3F;
    } else if(value == 1) {
        P0 = 0x06;
    } else if(value == 2) {
        P0 = 0x5B;
    } else if(value == 3) {
        P0 = 0x4F;
    } else if(value == 4) {
        P0 = 0x66;
    } else if(value == 5) {
        P0 = 0x6D;
    } else if(value == 6) {
        P0 = 0x7D;
    } else if(value == 7) {
        P0 = 0x07;
    } else if(value == 8) {
        P0 = 0x7F;
    } else if(value == 9) {
        P0 = 0x6F;
    }
}

void keypad_scan() {
    
    P2 = 0xF7; //P2.3
    if(COL1 == 0) {
        delay();
        if(COL1 == 0) {
            mode = 1;
            value = 1;
             while(COL1 == 0);
        }
        
    } else if(COL2 == 0) {
        delay();
        if(COL2 == 0) {
            mode = 4;
            value = 4;
             while(COL2 == 0);
        }
        
    } else if(COL3 == 0) {
        delay();
        if(COL3 == 0) {
            mode = 7;
            value = 7;
            while(COL3 == 0);
        }
    } 
    
    P2 = 0xFB; //P2.2
    if(COL1 == 0) {
        delay();
        if(COL1 == 0) {
            mode = 2;
            value = 2;
            while(COL1 == 0);
        }
    } else if(COL2 == 0) {
        delay();
        if(COL2 == 0) {
            mode = 5;
            value = 5;
            while(COL2 == 0);
        }
        
        
    } else if(COL3 == 0) {
        delay();
        if(COL3 == 0) {
            mode = 8;
            value = 8;
            while(COL3 == 0);
        }
        
        
    } else if(COL4 == 0) {
        delay();
        if(COL4 == 0) {
            mode = 9;
            value = 0;
            while(COL4 == 0);
        }
        
    }
    
    P2 = 0xFD;  //P2.1
    if(COL1 == 0) {
        delay();
        if(COL1 == 0) {
            mode = 3;
            value = 3;
            while(COL1 == 0); 
        }
    } else if(COL2 == 0) {
        delay();
        if(COL2 == 0) {
            mode = 6;
            value = 6;
            while(COL2 == 0);
        }
        
    } else if(COL3 == 0) {
        delay();
        if(COL3 == 0) {
            mode = 9;
            value = 9;
            P1 = 0x00;
            while(COL3 == 0);
        }
        
    }
    
    display_digit(value);
}

void main()
{
    P0 = 0x3F;
    P1 = 0x1E;
    P2 = 0xFF;

    while(1)
    {
        keypad_scan();

        switch(mode)
        {
            case 1:
                led_control(1);
                break;

            case 2:
                led_control(2);
                break;

            case 3:
                led_control(3);
                break;

            case 4:
                led_control(4);
                break;

            case 5:
                running_led();
                break;

            case 6:
                while(mode == 6) {
                    alternate_led();
                    keypad_scan();
                }
                break;

            case 7:
                binary_counter();
                break;

            case 8:
                blink_all();
                break;

            default:
                break;
        }
    }
}