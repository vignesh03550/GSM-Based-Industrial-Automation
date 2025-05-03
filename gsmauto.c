#include <lpc21xx.h>
#include <string.h>
#include "lcd.h"

#define MOTOR (1 << 16)
#define PUMP (1 << 18)
#define ALARM (1 << 21)

void UART_CONFIG(void);
void UART_TX(unsigned char ch);
void UART0_STR(const char *str);
unsigned char UART_RX(void);

int main(void) 
{
    char buffer[256], ch; // Buffer to store the received SMS
    int index = 0, startmsg = 0; // Flag to indicate the start of the message
    LCD_INIT();
    UART_CONFIG();
    IODIR0 |= MOTOR | PUMP | ALARM;
    //IOSET0 = MOTOR | PUMP | ALARM;
    LCD_COMMAND(0XC0);
    LCD_STR("V24CE3U1");
    LCD_COMMAND(0X80);
    LCD_SCROLL("GSM BASED INDUSTRIAL AUTOMATION CONTROL");

    // Initialize GSM module
    UART0_STR("AT\r\n"); // Test AT communication
    delay_milliseconds(1000);

    UART0_STR("AT+CMGF=1\r\n"); // Set SMS mode to text
    delay_milliseconds(1000);

    UART0_STR("AT+CNMI=1,2,0,0,0\r\n"); // Enable SMS notifications
    delay_milliseconds(1000);

    // Send SMS
    UART0_STR("AT+CMGS=\"+917569622947\"\r\n"); // Replace with your mobile number
    delay_milliseconds(1000);
    UART0_STR("MOTOR OFF\nPUMP OFF\nALARM OFF"); // Message content
    UART_TX(0x1a); // Send Ctrl+Z to send SMS
    delay_milliseconds(3000);

    // Wait for SMS and read it
    while (1) 
    {
        ch = UART_RX(); // Read a character from UART
	if(ch == '\n' || ch == '\r') 
        {  
            if(index == 0) 
	    continue; // Ignore empty lines

            buffer[index] = '\0'; // Null-terminate the string

            if(strstr(buffer, "+CMT:") != 0) 
            {
                // SMS header detected, next line contains the actual message
                startmsg = 1;
                index = 0; // Reset index for storing the message
                continue;
            }

            if(startmsg == 1) 
            {
                // Message received after header
                UART0_STR("Received SMS: ");
		UART0_STR(buffer); // Display the received SMS
		LCD_COMMAND(0X01);
		LCD_COMMAND(0x80);// display on LCD
		LCD_STR((unsigned char *)buffer);
                UART0_STR("\r\n");
                startmsg = 0; // Reset flag
            }

            index = 0; // Reset buffer index for next line
        } 
        else if (index < sizeof(buffer) - 1) 
        {
            buffer[index++] = ch; // Store character in buffer
        }

	if(strcmp(buffer, "MOTOR ON") == 0)// Motor control
	{
		IOSET0 = MOTOR;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "MOTOR OFF") == 0)
	{
		IOCLR0 = MOTOR;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "PUMP ON") == 0)// Pump control
	{	
		IOSET0 = PUMP;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "PUMP OFF") == 0)
	{
		
		IOCLR0 = PUMP;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "ALARM ON") == 0)// Alarm control
	{
		
		IOSET0 = ALARM;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "ALARM OFF") == 0)
	{ 
	   
		IOCLR0 = ALARM;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "ALL OFF") == 0)
	{	
    
	 	IOSET0 = MOTOR | PUMP | ALARM;
		delay_milliseconds(300);
	}
	else if(strcmp(buffer, "ALL ON") == 0)
	{  
	    
		IOCLR0 = MOTOR | PUMP | ALARM;
		delay_milliseconds(300);
	}	
    }
}

void UART_CONFIG(void) 
{
    PINSEL0 = 0X5;       // Configure P0.0 and P0.1 as TXD0 and RXD0
    U0LCR = 0X83;        // Enable DLAB to set baud rate
    U0DLL = 97;          // Set baud rate to 9600 (assuming 15 MHz clock)
    U0DLM = 0;
    U0LCR = 0X03;        // Disable DLAB
}

void UART_TX(unsigned char d) 
{
    while ((U0LSR & (1 << 5)) == 0); // Wait until THR is empty
    U0THR = d;
}

void UART0_STR(const char *s) 
{
    while (*s) 
        UART_TX(*s++);
}

unsigned char UART_RX(void) 
{
    while ((U0LSR & 0x01) == 0); // Wait until data is received
    return U0RBR;                // Return received data
}

