#include "eeprom.h"
#include <stdint.h>

/* GET functions */
uint8_t getPrescaler(void);
uint16_t getMinLight(void);
uint16_t getMaxLight(void);
uint16_t getMinTemp(void);
uint16_t getMaxTemp(void);
uint16_t getMaxPoll(void);
uint32_t getR1(void);
float getC1(void);
float getC2(void);
float getC3(void);

/* SET functions */
void setPrescaler(char* str);
void setMinLight(char* str);
void setMaxLight(char* str);
void setMinTemp(char* str);
void setMaxTemp(char* str);
void setMaxPoll(char* str);
void setR1(char* str);
void setC1(char* str);
void setC2(char* str);
void setC3(char* str);
