#include "delay.h"
#include <lcd_lib.h>
#include "packet_handler.h"
#include "uart.h"
#include <string.h>

#define BUF_SIZE 16
char buffer[BUF_SIZE];
volatile int size = 0; 

volatile int responseCompleted = 0;

#pragma pack(push, 1)
typedef struct Packet0{
	PacketHeader header;
	short val;
} Packet0;
#pragma pack(pop)

#define PACKET0_TYPE 0
#define PACKET0_SIZE (sizeof(Packet0))

PacketHandler packet_handler;

Packet0 packet0_buffer;

PacketHeader* Packet0_initializeBuffer(PacketType type, PacketSize size, void* args __attribute__((unused))) {
	if (type != PACKET0_TYPE || size != PACKET0_SIZE)
		return 0;
	return (PacketHeader*) &packet0_buffer;
}

PacketStatus Packet0_print(PacketHeader* header){
	Packet0* p =(Packet0 *) header;
	uint8_t c = (char) p->val;
	if (c==0 || size==BUF_SIZE-1){ // end of msg OR end of buffer: in both case print stored message
		LCDclr();
		LCDstring((uint8_t*)buffer, strlen(buffer));
		size = 0;
		responseCompleted = 1; // let's request next EEPROM param
		return Success;
	}
	buffer[size++] = c;
	buffer[size] = 0;
	return Success;
};

PacketStatus Packet0_onReceive(PacketHeader* header, void* args __attribute__((unused))){
	return Packet0_print(header);
}

PacketOperations packet0_ops = {
	0,
	sizeof(Packet0),
	Packet0_initializeBuffer,
	0,
	Packet0_onReceive,
	0
};

struct UART* uart;

void flushInputBuffers(void){
	while (UART_rxBufferFull(uart)){
		uint8_t c = UART_getChar(uart);
		PacketHandler_rxByte(&packet_handler, c);
	}
}

int flushOutputBuffers(void){
	while (packet_handler.tx_size)
		UART_putChar(uart, PacketHandler_txByte(&packet_handler));
	return packet_handler.tx_size;
}

void sendRequest(char* msg){
	int seq=0;
	int l = strlen(msg);
	//sending all chars of msg one by one
	for (int i=0; i<l; i++){
		Packet0 p0 = { {PACKET0_TYPE, PACKET0_SIZE, 0}, (short)msg[i] };
		p0.header.seq=seq;
		seq++;
		PacketHandler_sendPacket(&packet_handler, (PacketHeader*) &p0);
		delayMs(10);
	}
	//sending 0 as terminator character at the end of msg
	Packet0 p0 = { {PACKET0_TYPE, PACKET0_SIZE, 0}, 0 };
	p0.header.seq = seq;
	seq++;
	PacketHandler_sendPacket(&packet_handler, (PacketHeader*) &p0);
	delayMs(10);
}

int main(int argc, char** argv){
	uart = UART_init(0, 115200);
	
	LCDinit();
	LCDclr();
	LCDcursorOFF();
	
	buffer[0] = 0;
	
	PacketHandler_initialize(&packet_handler);
	PacketHandler_installPacket(&packet_handler, &packet0_ops);	
	
	delayMs(2000);

	while (1){
		sendRequest("timer in ms?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();
		
		responseCompleted = 0;
		sendRequest("min light?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();
		
		responseCompleted = 0;
		sendRequest("max light?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();
		
		responseCompleted = 0;
		sendRequest("min temperature?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();
		
		responseCompleted = 0;
		sendRequest("max temperature?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();
		
		responseCompleted = 0;
		sendRequest("max pollution?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();
		
		responseCompleted = 0;
		sendRequest("thermistor resistance?");
		flushOutputBuffers();
		while (!responseCompleted)
			flushInputBuffers();

		delayMs(100);
		break;
	}
}
