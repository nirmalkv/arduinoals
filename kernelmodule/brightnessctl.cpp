#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"

using namespace std;
string const backlightdevicepath ="/sys/class/backlight/intel_backlight/";

// Funtion to get maximum brigtness device support
int getScreenBacklightMax()
{
    string path = backlightdevicepath + "max_brightness";
    int fin = open(path.c_str(), O_RDONLY);
    char str[4];
    int count = read(fin, str, sizeof(str));
    str[count] = '\0';
    close(fin);
    int value = atoi(str);
    return value;
}
// To get actual brightness 
int getScreenActuallightMax()
{
    string path = backlightdevicepath  + "actual_brightness";
    int fin = open(path.c_str(), O_RDONLY);
    char str[4];
    int count = read(fin, str, sizeof(str));
    str[count] = '\0';
    close(fin);
    int value = atoi(str);
    return value;
}

int main()
{
    int dev_fd;
    char str[16];
    // Open platform device to communicate to the kernel 
    dev_fd = open("/sys/devices/platform/autobrightness/alsevent", O_RDWR);
    if (dev_fd < 0) {
        perror("Couldn't open alsdriver platform device\n");
        exit(-1);
    }
    int fd,c,res;
    int maxBrightness = getScreenBacklightMax();
    int minBrightness = 75;

    // To read serial data from usb device ttyUSB0
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
    
    int i,sum,avgVal = 0,sensorVal,change,prevBrightness;

    // read from usb device and write to the platform device 
    while(1){
        i = 20;
        sum = 0;
        while(i>0){
            res = read(fd,buf,255); 
            buf[res]=0;
            sensorVal = atoi(buf);
            if(sensorVal > 1000){
                sensorVal = maxBrightness;
            }
            else if (sensorVal < 150){
                sensorVal = minBrightness;
            }
            sum = sum + sensorVal;
            i--;
        }
        avgVal = sum / 20;
        prevBrightness = getScreenActuallightMax();
        sprintf(str,"%d %d %d %d",avgVal, minBrightness, maxBrightness, prevBrightness);
        write(dev_fd, str, sizeof(str));
	    fsync(dev_fd);
    }
    close(fd);
    return 0;
}
