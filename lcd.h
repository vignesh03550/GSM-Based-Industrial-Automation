 #include <LPC21xx.H>

#define LCD_D  (0x0F << 20)
#define RS     (1 << 17)
#define E      (1 << 19)
#define RW     (1 << 18)

void LCD_INIT(void);
void LCD_COMMAND(unsigned char cmd);
void LCD_DATA(unsigned char d);
void delay_milliseconds(unsigned int ms);
void LCD_STR(unsigned char *d);

void LCD_INIT(void)
{
    IODIR1 = LCD_D | RS | E | RW;

    LCD_COMMAND(0x01);
    LCD_COMMAND(0x02);
    LCD_COMMAND(0x0C);
    LCD_COMMAND(0x28);
    LCD_COMMAND(0x80);
}

void LCD_COMMAND(unsigned char cmd)
{
    IOCLR1 = RS;
    IOCLR1 = LCD_D;
    
    IOSET1 = (cmd & 0xF0) << 16;
    IOSET1 = E;
    delay_milliseconds(2);
    IOCLR1 = E;
    
    IOCLR1 = LCD_D;
    IOSET1 = (cmd & 0x0F) << 20;
    IOSET1 = E;
    delay_milliseconds(2);
    IOCLR1 = E;
}

void LCD_DATA(unsigned char d)
{
    IOSET1 = RS;
    IOCLR1 = LCD_D;
    
    IOSET1 = (d & 0xF0) << 16;
    IOSET1 = E;
    delay_milliseconds(2);
    IOCLR1 = E;
    
    IOCLR1 = LCD_D;
    IOSET1 = (d & 0x0F) << 20;
    IOSET1 = E;
    delay_milliseconds(2);
    IOCLR1 = E;
}

void delay_milliseconds(unsigned int ms)
{
    T0PR = 15000 - 1;
    T0TCR = 0x01;
    while (T0TC < ms);
    T0TCR = 0x03;
    T0TCR = 0x00;
}

void LCD_STR(unsigned char *d)
{
    while (*d != '\0')
    {
        LCD_DATA(*d++);
    }
}

void LCD_SCROLL(char *s) 


{

	int len = 0, i, j;

	while(s[len] != '\0') 

	len++;

	for (i= 0; i <= len; i++) 

	{

		LCD_COMMAND(0x80);

		for (j= 0; j < 16; j++) 

		{

			if ((i + j) < len)

			LCD_DATA(s[i + j]);

			else

			LCD_DATA(' ');

		}

		delay_milliseconds(300);

	}

}
