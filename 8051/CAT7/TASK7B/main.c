// Include the common LCD driver functions here
#include <reg52.h>

#define LCD_PORT P2

sbit RS = P2^0;
sbit EN = P2^1;

// Delay function
void delay(unsigned int t)
{
    unsigned int i, j;

    for(i = 0; i < t; i++)
        for(j = 0; j < 120; j++);
}

// Send 4 bits to LCD
void lcd_nibble(unsigned char value)
{
    LCD_PORT = (LCD_PORT & 0x0F) | (value & 0xF0);

    EN = 1;
    delay(1);
    EN = 0;
}

// Send LCD command
void lcd_cmd(unsigned char cmd)
{
    RS = 0;

    lcd_nibble(cmd);
    lcd_nibble(cmd << 4);

    delay(2);
}

// Send character
void lcd_data(unsigned char dat)
{
    RS = 1;

    lcd_nibble(dat);
    lcd_nibble(dat << 4);

    delay(2);
}

// Initialize LCD in 4-bit mode
void lcd_init(void)
{
    delay(20);

    RS = 0;
    lcd_nibble(0x30);
    delay(5);

    lcd_nibble(0x30);
    delay(1);

    lcd_nibble(0x30);
    delay(1);

    lcd_nibble(0x20);

    lcd_cmd(0x28);  // 4-bit, 2-line mode
    lcd_cmd(0x0C);  // Display ON, cursor OFF
    lcd_cmd(0x06);  // Increment cursor
    lcd_cmd(0x01);  // Clear display

    delay(5);
}

// Set cursor position
void lcd_goto(unsigned char row, unsigned char column)
{
    if(row == 0)
        lcd_cmd(0x80 + column);
    else
        lcd_cmd(0xC0 + column);
}

// Display string
void lcd_puts(char *str)
{
    while(*str)
    {
        lcd_data(*str);
        str++;
    }
}

// Display integer
void lcd_print_number(unsigned int num)
{
    unsigned char digits[5];
    unsigned char i = 0;

    if(num == 0)
    {
        lcd_data('0');
        return;
    }

    while(num > 0)
    {
        digits[i] = (num % 10) + '0';
        num = num / 10;
        i++;
    }

    while(i > 0)
    {
        i--;
        lcd_data(digits[i]);
    }
}

// Clear LCD
void lcd_clear(void)
{
    lcd_cmd(0x01);
    delay(5);
}



void main(void)
{
    unsigned int count = 0;

    lcd_init();

    lcd_goto(0, 0);
    lcd_puts("Counter Value:");

    while(1)
    {
        lcd_goto(1, 0);
        lcd_puts("Value: ");

        // Clear previous number
        lcd_puts("     ");

        // Move cursor back to number position
        lcd_goto(1, 7);

        // Display updated number
        lcd_print_number(count);

        count++;

        if(count > 9999)
        {
            count = 0;
        }

        delay(500);
    }
}

