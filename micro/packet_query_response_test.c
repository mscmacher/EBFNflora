#include "delay.h"
#include "lcd_lib.h"
#include "packet_handler.h"
#include "uart.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

PacketHandler packet_handler;

/* -------------------- TimerConfigPacket -------------------- */

#pragma pack(push, 1)
typedef struct TimerConfigPacket{
	PacketHeader header;
	uint32_t duration;
} TimerConfigPacket;
#pragma pack(pop)

#define TIMER_CONFIG_PACKET_TYPE 0
#define TIMER_CONFIG_PACKET_SIZE (sizeof(TimerConfigPacket))

TimerConfigPacket timer_config_packet_buffer;

PacketHeader* TimerConfigPacket_initializeBuffer(PacketType type, PacketSize size, void* args __attribute__((unused))) {
	if (type != TIMER_CONFIG_PACKET_TYPE || size != TIMER_CONFIG_PACKET_SIZE)
		return 0;
	return (PacketHeader*) &timer_config_packet_buffer;
}

PacketStatus TimerConfigPacket_onReceive(PacketHeader* header, void* args __attribute__((unused))){
	TimerConfigPacket* p = (TimerConfigPacket*) header;
	int duration = p->duration;
	char buffer[8];
	sprintf(buffer, "%d", duration);
	LCDclr();
	LCDstring((uint8_t*)buffer, strlen(buffer));
	return Success;
}

PacketOperations TimerConfigPacket_ops = {
	TIMER_CONFIG_PACKET_TYPE,
	sizeof(TimerConfigPacket),
	TimerConfigPacket_initializeBuffer,
	0,
	TimerConfigPacket_onReceive,
	0
};

/* -------------------- LightConfigPacket -------------------- */

#pragma pack(push, 1)
typedef struct LightConfigPacket{
	PacketHeader header;
	uint16_t minLight;
	uint16_t maxLight;
} LightConfigPacket;
#pragma pack(pop)

#define LIGHT_CONFIG_PACKET_TYPE 1
#define LIGHT_CONFIG_PACKET_SIZE (sizeof(LightConfigPacket))

LightConfigPacket light_config_packet_buffer;

PacketHeader* LightConfigPacket_initializeBuffer(PacketType type, PacketSize size, void* args __attribute__((unused))) {
	if (type != LIGHT_CONFIG_PACKET_TYPE || size != LIGHT_CONFIG_PACKET_SIZE)
		return 0;
	return (PacketHeader*) &light_config_packet_buffer;
}

PacketStatus LightConfigPacket_onReceive(PacketHeader* header, void* args __attribute__((unused))){
	LightConfigPacket* p = (LightConfigPacket*) header;
	int minLight = p->minLight;
	int maxLight = p->maxLight;
	char buffer[16];
	sprintf(buffer, "%d, %d", minLight, maxLight);
	LCDclr();
	LCDstring((uint8_t*)buffer, strlen(buffer));
	return Success;
}

PacketOperations LightConfigPacket_ops = {
	LIGHT_CONFIG_PACKET_TYPE,
	sizeof(LightConfigPacket),
	LightConfigPacket_initializeBuffer,
	0,
	LightConfigPacket_onReceive,
	0
};

/* -------------------- TemperatureConfigPacket -------------------- */

#pragma pack(push, 1)
typedef struct TemperatureConfigPacket{
	PacketHeader header;
	uint16_t minTemp;
	uint16_t maxTemp;
	uint32_t R1;
} TemperatureConfigPacket;
#pragma pack(pop)

#define TEMPERATURE_CONFIG_PACKET_TYPE 2
#define TEMPERATURE_CONFIG_PACKET_SIZE (sizeof(TemperatureConfigPacket))

TemperatureConfigPacket temperature_config_packet_buffer;

PacketHeader* TemperatureConfigPacket_initializeBuffer(PacketType type, PacketSize size, void* args __attribute__((unused))) {
	if (type != TEMPERATURE_CONFIG_PACKET_TYPE || size != TEMPERATURE_CONFIG_PACKET_SIZE)
		return 0;
	return (PacketHeader*) &temperature_config_packet_buffer;
}

PacketStatus TemperatureConfigPacket_onReceive(PacketHeader* header, void* args __attribute__((unused))){
	TemperatureConfigPacket* p = (TemperatureConfigPacket*) header;
	int minTemp = p->minTemp;
	int maxTemp = p->maxTemp;
	int R1 = p->R1;
	char buffer[16];
	sprintf(buffer, "%d, %d", minTemp, maxTemp);
	LCDclr();
	LCDstring((uint8_t*)buffer, strlen(buffer));
	sprintf(buffer, "%d", R1);
	LCDGotoXY(0,1);
	LCDstring((uint8_t*)buffer, strlen(buffer));
	return Success;
}

PacketOperations TemperatureConfigPacket_ops = {
	TEMPERATURE_CONFIG_PACKET_TYPE,
	sizeof(TemperatureConfigPacket),
	TemperatureConfigPacket_initializeBuffer,
	0,
	TemperatureConfigPacket_onReceive,
	0
};

/* -------------------- PollutionConfigPacket -------------------- */

#pragma pack(push, 1)
typedef struct PollutionConfigPacket{
	PacketHeader header;
	uint16_t maxPoll;
} PollutionConfigPacket;
#pragma pack(pop)

#define POLLUTION_CONFIG_PACKET_TYPE 3
#define POLLUTION_CONFIG_PACKET_SIZE (sizeof(PollutionConfigPacket))

PollutionConfigPacket pollution_config_packet_buffer;

PacketHeader* PollutionConfigPacket_initializeBuffer(PacketType type, PacketSize size, void* args __attribute__((unused))) {
	if (type != POLLUTION_CONFIG_PACKET_TYPE || size != POLLUTION_CONFIG_PACKET_SIZE)
		return 0;
	return (PacketHeader*) &pollution_config_packet_buffer;
}

PacketStatus PollutionConfigPacket_onReceive(PacketHeader* header, void* args __attribute__((unused))){
	PollutionConfigPacket* p = (PollutionConfigPacket*) header;
	int maxPoll = p->maxPoll;
	char buffer[8];
	sprintf(buffer, "%d", maxPoll);
	LCDclr();
	LCDstring((uint8_t*)buffer, strlen(buffer));
	return Success;
}

PacketOperations PollutionConfigPacket_ops = {
	POLLUTION_CONFIG_PACKET_TYPE,
	sizeof(PollutionConfigPacket),
	PollutionConfigPacket_initializeBuffer,
	0,
	PollutionConfigPacket_onReceive,
	0
};

struct UART* uart;

void flushInputBuffers(void){
	while (UART_rxBufferFull(uart)){
		uint8_t c = UART_getChar(uart);
		PacketHandler_rxByte(&packet_handler, c);
	}
}

int main(int argc, char** argv){
	uart = UART_init(0, 115200);
	
	LCDinit();
	LCDclr();
	LCDcursorOFF();
	
	PacketHandler_initialize(&packet_handler);
	PacketHandler_installPacket(&packet_handler, &TimerConfigPacket_ops);
	PacketHandler_installPacket(&packet_handler, &LightConfigPacket_ops);
	PacketHandler_installPacket(&packet_handler, &TemperatureConfigPacket_ops);
	PacketHandler_installPacket(&packet_handler, &PollutionConfigPacket_ops);

	while (1){
		flushInputBuffers();
	}
}
