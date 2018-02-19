// http://timmurphy.org/2009/08/04/baud-rate-and-other-serial-comm-settings-in-c/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>    
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 300
speed_t baud = B115200; /* baud rate */

int main() {
	char buf[BUF_SIZE];
	int fd = open("/dev/ttyACM0", O_RDWR);
	if (fd < 0) {
		perror("error");
		exit(-1);
	}
	
	/* set the other settings */
	struct termios settings;
	tcgetattr(fd, &settings);

	cfsetospeed(&settings, baud); /* baud rate */
	settings.c_cflag &= ~PARENB; /* no parity */
	settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
	settings.c_cflag &= ~CSIZE;
	settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
	settings.c_lflag = ICANON; /* canonical mode */
	settings.c_oflag &= ~OPOST; /* raw output */

	tcsetattr(fd, TCSANOW, &settings); /* apply the settings */
	tcflush(fd, TCOFLUSH);
	
	int bytes_read = 0;
	while (1) {
		int n = read(fd, buf+bytes_read, 1);
		if (n <= 0) {
			perror("error");
			exit(-1);
		}
		if (buf[bytes_read] == '\0') {
			printf("\n");
		}
		printf("%c", buf[bytes_read += (bytes_read%BUF_SIZE)]); /* circular buffer */
	}
	
	return 0;
}
