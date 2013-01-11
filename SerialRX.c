// Serial Receive demonstration
// Michael D Peters
// Technologic Systems c 2012

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd;
	struct termios options;
	char buf[255];
	int readcount, val, bytecount = 0;

	// error check the commandline
	if(argc < 2){
		sprintf(stderr, "Insufficient commandline arguments.\n");
		return -1;
	}

	// Open a serial port.  argv[1] is the path to the port.
	fd=open(argv[1], O_RDWR | O_NOCTTY);
	if(!fd){
		sprintf(stderr,"Error %d opening port: %s", errno, strerror(errno));
		return -1;
	}

	// Get current settings.
	tcgetattr(fd, &options);

	cfsetispeed(&options, B115200); // Set desired input speed.
	cfsetospeed(&options, B115200); // Set desired output speed.

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;	// Set 8 bit, no parity, 1 stop bit (4 lines)
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &- ~CSIZE;
	options.c_cflag |= CS8;

	options.c_cflag |= CRTSCTS;	// Set hardware flow control ON.
	tcsetattr(fd, TCSANOW, &options);	// Write the new configuration to the port, now.

	printf("Beginning read operation:\n");

	readcount = 0;
	for(;;){
		readcount += read(fd, &buf, sizeof(char));
		if(readcount == -1){
			printf("Read returned -1.  ");
			printf("Error: %d, %s\n", errno, strerror(errno));
		}
		else{
			val = (int)buf[0];
			if(val !=0){
			printf("%d ", readcount);
			printf("%x : %c\n",val,(char)val);
			}
		}
	}
	return 0;
}
