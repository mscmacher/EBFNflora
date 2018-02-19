#pragma once
#include <stdint.h>

/* initialize adc with prescaler = 128 */
void adc_init(void);

/* initialize adc with a specific prescaler */
void adc_init_with_prescaler(uint8_t prescaler);

/* enable ADC_vect Interrupt */
void adc_enableInterrupt(void);

/* read adc value from channel ch */
uint16_t adc_read(uint8_t ch);
