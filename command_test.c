#include "lcd_lib.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 32

char buffer[BUFFER_SIZE];
volatile int size = 0;

struct UART* uart; 

ISR(USART_RX_vect) {
	uint8_t c = UDR0;
	buffer[size++] = c;
	buffer[size] = 0;
	
	//print string on LCD screen
	if(c == 0){
		if (strcmp(buffer, "pianta") == 0) {
			LCDclr();
			LCDstring((uint8_t*)buffer, strlen(buffer));
		}
		else if (strcmp(buffer, "pianta grassa") == 0) {
			LCDclr();
			LCDstring((uint8_t*)buffer, strlen(buffer));
		}
		memset(buffer, 0, size);
		size = 0;
	}
}

int main(void){	
	uart = UART_init("uart_0", 115200);
	buffer[0] = 0;
	
	//initialize LCD screen
	LCDinit();
	LCDclr();
	LCDcursorOFF();
	
	while(1);
}
