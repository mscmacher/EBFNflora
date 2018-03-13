#include "delay.h"
#include "lcd_lib.h"
#include "packet_handler.h"
#include "uart.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

PacketHandler packet_handler;

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
	0,
	sizeof(TimerConfigPacket),
	TimerConfigPacket_initializeBuffer,
	0,
	TimerConfigPacket_onReceive,
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

	while (1){
		flushInputBuffers();
	}
}
