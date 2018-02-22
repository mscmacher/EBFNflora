#include "eepromParams.h"
#include "delay.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

struct UART* uart;

void printString(char* s){
	int l=strlen(s);
	for(int i=0; i<l; ++i, ++s)
		UART_putChar(uart, (uint8_t) *s);
}

char* frequency = "125K";
char* minLight = "50";
char* maxLight = "700";
char* minTemp = "15";
char* maxTemp  = "55";
char* maxPoll = "150";
char* R1 = "13000";
char* C1 = "1.009249522e-03";
char* C2 = "2.378405444e-04";
char* C3 = "2.019202697e-07";

int main(void){
	EEPROM_init();
	uart = UART_init("uart_0",115200);
	
	setPrescaler(frequency);
	setMinLight(minLight);
	setMaxLight(maxLight);
	setMinTemp(minTemp);
	setMaxTemp(maxTemp);
	setMaxPoll(maxPoll);
	setR1(R1);
	setC1(C1);
	setC2(C2);
	setC3(C3);
	
	while(1){
		char msg[16];
		
		int fClock = getPrescaler();
		sprintf(msg, "%d\n", fClock);
		printString(msg);
		delayMs(300);
		
		int minLightValue = getMinLight();
		sprintf(msg, "%d\n", minLightValue);
		printString(msg);
		delayMs(300);
		
		int maxLightValue = getMaxLight();
		sprintf(msg, "%d\n", maxLightValue);
		printString(msg);
		delayMs(300);
		
		int minTempValue = getMinTemp();
		sprintf(msg, "%d\n", minTempValue);
		printString(msg);
		delayMs(300);
		
		int maxTempValue = getMaxTemp();
		sprintf(msg, "%d\n", maxTempValue);
		printString(msg);
		delayMs(300);
		
		int maxPollValue = getMaxPoll();
		sprintf(msg, "%d\n", maxPollValue);
		printString(msg);
		delayMs(300);
		
		int r1Value = getR1();
		sprintf(msg, "%d\n", r1Value);
		printString(msg);
		delayMs(300);
		
		float c1Value = getC1();
		sprintf(msg, "%f\n", c1Value);
		printString(msg);
		delayMs(300);
		
		float c2Value = getC2();
		sprintf(msg, "%f\n", c2Value);
		printString(msg);
		delayMs(300);
		
		float c3Value = getC3();
		sprintf(msg, "%f\n", c3Value);
		printString(msg);
		delayMs(300);
	}
}
