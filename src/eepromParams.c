#include "eepromParams.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

struct UART* uart;

/* GET functions */
uint8_t getPrescaler(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 0, BLOCK_SIZE);
	
	if (strcmp(eeprom_buffer, "16M") == 0) return 0;
	else if (strcmp(eeprom_buffer, "8M") == 0) return 2;
	else if (strcmp(eeprom_buffer, "4M") == 0) return 4;
	else if (strcmp(eeprom_buffer, "2M") == 0) return 8;
	else if (strcmp(eeprom_buffer, "1M") == 0) return 16;
	else if (strcmp(eeprom_buffer, "500K") == 0) return 32;
	else if (strcmp(eeprom_buffer, "250K") == 0) return 64;
	else if (strcmp(eeprom_buffer, "125K") == 0) return 128;
	else return 128; // default prescaler 
}

uint16_t getMinLight(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 16, BLOCK_SIZE);
	return atoi(eeprom_buffer);
}

uint16_t getMaxLight(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 32, BLOCK_SIZE);
	return atoi(eeprom_buffer);
}

uint16_t getMinTemp(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 48, BLOCK_SIZE);
	return atoi(eeprom_buffer);
}

uint16_t getMaxTemp(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 64, BLOCK_SIZE);
	return atoi(eeprom_buffer);
}

uint16_t getMaxPoll(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 80, BLOCK_SIZE);
	return atoi(eeprom_buffer);
}

uint32_t getR1(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 96, BLOCK_SIZE);
	return atoi(eeprom_buffer);
}

float getC1(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 112, BLOCK_SIZE);
	return atof(eeprom_buffer);
}

float getC2(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 128, BLOCK_SIZE);
	return atof(eeprom_buffer);
}

float getC3(void){
	char eeprom_buffer[BLOCK_SIZE];
	memset(eeprom_buffer, 0, BLOCK_SIZE);
	EEPROM_read(eeprom_buffer, 144, BLOCK_SIZE);
	return atof(eeprom_buffer);
}

/* SET functions */
void setPrescaler(char* str){
	// we write frequency on eeprom at address 0
	int sizeFrequency = strlen(str)+1;
	EEPROM_write(0, str, sizeFrequency);
}

void setMinLight(char* str){
	// we write minLight on eeprom at address 16
	int sizeMinLight = strlen(str)+1;
	EEPROM_write(16, str, sizeMinLight);
}

void setMaxLight(char* str){
	// we write maxLight on eeprom at address 32
	int sizeMaxLight = strlen(str)+1;
	EEPROM_write(32, str, sizeMaxLight);
}

void setMinTemp(char* str){
	// we write minTemp on eeprom at address 48
	int sizeMinTemp = strlen(str)+1;
	EEPROM_write(48, str, sizeMinTemp);
}

void setMaxTemp(char* str){
	// we write maxTemp on eeprom at address 64
	int sizeMaxTemp = strlen(str)+1;
	EEPROM_write(64, str, sizeMaxTemp);
}

void setMaxPoll(char* str){
	// we write maxPoll on eeprom at address 80
	int sizeMaxPoll = strlen(str)+1;
	EEPROM_write(80, str, sizeMaxPoll);
}

void setR1(char* str){
	// we write R1 on eeprom at address 96
	int sizeR1 = strlen(str)+1;
	EEPROM_write(96, str, sizeR1);
}

void setC1(char* str){
	// we write C1 on eeprom at address 112
	int sizeC1 = strlen(str)+1;
	EEPROM_write(112, str, sizeC1);
}

void setC2(char* str){
	// we write C2 on eeprom at address 128
	int sizeC2 = strlen(str)+1;
	EEPROM_write(128, str, sizeC2);
}

void setC3(char* str){
	// we write C3 on eeprom at address 144
	int sizeC3 = strlen(str)+1;
	EEPROM_write(144, str, sizeC3);
}
