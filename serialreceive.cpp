#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <iostream>

using namespace std;
#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1

int main(){
    int fd,c,res;
    struct termios oldtio,newtio;
    char buf[255];

    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(MODEMDEVICE); exit(-1); } 

    tcgetattr(fd,&oldtio);
    bzero(&newtio, sizeof(newtio));


    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
 
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    int i,sum;
    while(1){
        i = 50;
        sum = 0;
        while(i>0){
            res = read(fd,buf,255); 
            buf[res]=0;
            sum = sum + atoi(buf);
            i--;
        }
        cout << sum/50<<"\n";
    }
    close(fd);
}