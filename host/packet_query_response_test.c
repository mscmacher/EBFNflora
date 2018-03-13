#include "packet_handler.h"
#include "serial_linux.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#define NUM_EEPROM_PARAMS 7

#define STR_LENGTH 16

#define BUF_SIZE 32
char buffer[BUF_SIZE];
volatile int size = 0; 

volatile int requestCompleted = 0;

#pragma pack(push, 1)
typedef struct Packet0{
	PacketHeader header;
	short val;
} Packet0;
#pragma pack(pop)

#define PACKET0_TYPE 0
#define PACKET0_SIZE (sizeof(Packet0))

Packet0 p0 = { {PACKET0_TYPE, PACKET0_SIZE, 0}, 0 };

PacketHandler packet_handler;

Packet0 packet0_buffer;

PacketHeader* Packet0_initializeBuffer(PacketType type, PacketSize size, void* args __attribute__((unused))){
	if (type != PACKET0_TYPE || size != PACKET0_SIZE)
		return 0;
	return (PacketHeader*) &packet0_buffer;
}

PacketStatus Packet0_print(PacketHeader* header){
	Packet0* p =(Packet0 *) header;
	uint8_t c = (char) p->val;
	if (c==0 || size==BUF_SIZE-1){ // end of msg OR end of buffer: in both case print stored message
		printf("%s\n", buffer);
		size = 0;
		requestCompleted = 1;
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

void sendResponse(char* msg){
	int seq = 0;
	int l = strlen(msg);
	//sending all chars of msg one by one
	for (int i=0; i<l; i++){
		Packet0 p0 = { {PACKET0_TYPE, PACKET0_SIZE, 0}, (short)msg[i] };
		p0.header.seq=seq;
		seq++;
		PacketHandler_sendPacket(&packet_handler, (PacketHeader*) &p0);
		usleep(10000);
	}
	//sending 0 as terminator character at the end of msg
	Packet0 p0 = { {PACKET0_TYPE, PACKET0_SIZE, 0}, 0 };
	p0.header.seq = seq;
	seq++;
	PacketHandler_sendPacket(&packet_handler, (PacketHeader*) &p0);
	usleep(10000);
}

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
		
	buffer[0] = 0;
	char inputString[STR_LENGTH];
	
	PacketHandler_initialize(&packet_handler);
	PacketHandler_installPacket(&packet_handler, &packet0_ops);
	
	int current_param = 0;
	while(current_param < NUM_EEPROM_PARAMS){ //end cycle when all EEPROM params are set
		volatile int packet_complete = 0;
		while (! packet_complete){
			uint8_t c;
			int n = read (fd, &c, 1);
			if (n){
				PacketStatus status = PacketHandler_rxByte(&packet_handler, c);
				packet_complete = (status == SyncChecksum);
			}
		}
		
		if (requestCompleted){
			//getting new EEPROM param from stdin
			fgets(inputString, STR_LENGTH, stdin);
			if((strlen(inputString) > 0) && (inputString[strlen(inputString) - 1] == '\n'))
				inputString[strlen(inputString) - 1] = '\0';
			
			//sending to MCU
			sendResponse(inputString);
			while (packet_handler.tx_size){
				uint8_t c = PacketHandler_txByte(&packet_handler);
				write(fd, &c, 1);
				usleep(1000);
			}
			
			current_param++;
			requestCompleted = 0;
		}
	}
}
