// Writes a character sequence to the first
//   serial port on the command line.
// Reads second port expecting same back.

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  int fd, fd2, i, j;
  struct termios options;
  char buf[256] = {0x55};
  char buf2[256] = {0};
  int readcount, val, bytecount = 0;
  int count = 0;

  // error check the commandline
  if(argc < 3){
    fprintf(stderr, "Insufficient commandline arguments.  Want 2 file handles.\n");
    return 1;
  }

  // Open a serial port.  argv[1] is the path to the port.
  fd=open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(fd == -1){
    fprintf(stderr,"Error %d opening port: %s", errno, strerror(errno));
    return 1;
  }
  // Open another serial port.  argv[2] is the path to the port.
  fd2=open(argv[2], O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(fd2 == -1){
    fprintf(stderr,"Error %d opening port: %s", errno, strerror(errno));
    return 1;
  }

  // Get current settings.
  tcgetattr(fd, &options);

  cfsetispeed(&options, B115200); // Set desired input speed.
  cfsetospeed(&options, B115200); // Set desired output speed.

  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB; // Set 8 bit, no parity, 1 stop bit (4 lines)
  options.c_iflag = IGNPAR;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  options.c_oflag = 0;
  options.c_lflag = 0;

  tcsetattr(fd, TCSANOW, &options); // Write the new configuration to the port, now.

  tcgetattr(fd2, &options);

  cfsetispeed(&options, B115200); // Set desired input speed.
  cfsetospeed(&options, B115200); // Set desired output speed.

  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB; // Set 8 bit, no parity, 1 stop bit (4 lines)
  options.c_iflag = IGNPAR;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  options.c_oflag = 0;
  options.c_lflag = 0;

  tcsetattr(fd2, TCSANOW, &options); // ... and same to the next port.

  printf("Beginning operation:\n");
  for(j=0; j<1000; j++){
      // reset test variables.
      readcount = 0;
      for(i=0; i<255; i++) buf[i] = (unsigned char)i;
      for(i=0; i<255; i++) buf2[i] = 0;

      // Place write code here.
      count = write(fd, buf, sizeof(char)*255);
      if(count < 0) {
        fprintf(stderr,"Error %d writing output:  %s\n", errno, strerror(errno));
        return 1;
      }

      usleep(100000);

      //printf("Cleared buffer.  Beginning read.\n");

      // read it back here.
      readcount = read(fd2, &buf2, sizeof(char)*255);
      if(readcount == -1){
        fprintf(stderr,"Error %d reading input: %s\n", errno, strerror(errno));
        return 1;
      }

    // verify?
    if(readcount != 0){
        printf("readcount=%d\n", readcount);
      printf("loopcount=%d\n", j);
      for(i = 0; i < readcount; i++){
        if(buf2[i] != buf[i]){
          printf("Loopback Fail:  Unexpected RX data at buf2[%d]:  0x%2X.\n", i, buf2[i]);
          return 1;
        }
        else if(i==0x55)
          printf("85th char is %C, as expected.\n",buf2[i]);
      }
    }
    else {
        printf("Loopback fail:  Readcount zero.\n");
      return 1;
    }
    printf("Transmission verified.  Buffer 2 last word is 0x%2X %2X, as expected.\n", buf2[253], buf2[254]);
  }
  return 0;
}
