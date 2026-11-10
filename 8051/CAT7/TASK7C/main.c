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




#include <reg52.h>

// Keypad connected to Port 1
// Rows: P1.0 to P1.3
// Columns: P1.4 to P1.7

// Scan keypad
char keypad_getkey(void)
{
    unsigned char row, col;
    unsigned char value;

    char keys[4][4] =
    {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for(row = 0; row < 4; row++)
    {
        P1 = 0xFF;

        // Make one row LOW
        P1 = 0xF0 | (0x0F & ~(1 << row));

        value = P1;

        for(col = 0; col < 4; col++)
        {
            if((value & (0x10 << col)) == 0)
            {
                return keys[row][col];
            }
        }
    }

    return 0;
}

void main(void)
{
    unsigned char row = 0;
    unsigned char col = 0;

    char key;

    lcd_init();

    // Initial character position
    lcd_goto(row, col);
    lcd_data('X');

    P1 = 0xFF;

    while(1)
    {
        key = keypad_getkey();

        if(key != 0)
        {
            delay(20);  // Debouncing

            if(keypad_getkey() == key)
            {
                // Erase old character
                lcd_goto(row, col);
                lcd_data(' ');

                // Move according to key
                if(key == '2' && row > 0)
                {
                    row--;
                }

                else if(key == '8' && row < 1)
                {
                    row++;
                }

                else if(key == '4' && col > 0)
                {
                    col--;
                }

                else if(key == '6' && col < 15)
                {
                    col++;
                }

                // Display at new position
                lcd_goto(row, col);
                lcd_data('X');

                // Wait until key is released
                while(keypad_getkey() != 0);

                delay(20);
            }
        }
    }
}
