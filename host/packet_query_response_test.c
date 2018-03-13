#include "packet_handler.h"
#include "serial_linux.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STR_LENGTH 16

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

PacketOperations TimerConfigPacket_ops = {
	0,
	sizeof(TimerConfigPacket),
	TimerConfigPacket_initializeBuffer,
	0,
	0,
	0
};

struct UART* uart;

int main(int argc, char** argv){
	assert(argc > 1);
	int fd = serial_open(argv[1]);
	if (fd < 0)
		return 0;
	if (serial_set_interface_attribs(fd, 115200, 0) < 0)
		return 0;
	serial_set_blocking(fd, 1); 
	if  (! fd)
		return 0;
		
	char inputString[STR_LENGTH];
	
	PacketHandler_initialize(&packet_handler);
	PacketHandler_installPacket(&packet_handler, &TimerConfigPacket_ops);
	
	while(1){
		//getting timer duration from stdin
		printf("Insert timer duration: ");
		fgets(inputString, STR_LENGTH, stdin);
		if((strlen(inputString) > 0) && (inputString[strlen(inputString) - 1] == '\n'))
			inputString[strlen(inputString) - 1] = '\0';
		
		uint32_t duration = atoi(inputString);
		TimerConfigPacket p0 = { {TIMER_CONFIG_PACKET_TYPE, TIMER_CONFIG_PACKET_SIZE, 0}, duration };
		
		//sending to MCU
		PacketHandler_sendPacket(&packet_handler, (PacketHeader*) &p0);
		while (packet_handler.tx_size){
			uint8_t c = PacketHandler_txByte(&packet_handler);
			write(fd, &c, 1);
			usleep(1000);
		}
		
		break;
	}
}
