#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;
#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1
string const backlightdevicepath ="/sys/class/backlight/intel_backlight/";

int getScreenBacklightMax()
{
    string path = backlightdevicepath + "max_brightness";
    int fin = open(path.c_str(), O_RDONLY);
    char str[4];
    int count = read(fin, str, sizeof(str));
    if (count == -1) return 0;
    str[count] = '\0';
    close(fin);
    int value = atoi(str);
    return value;
}

int getScreenActuallightMax()
{
    string path = backlightdevicepath  + "actual_brightness";
    int fin = open(path.c_str(), O_RDONLY);
    char str[4];
    int count = read(fin, str, sizeof(str));
    if (count == -1) return 0;
    str[count] = '\0';
    close(fin);
    int value = atoi(str);
    return value;
}

int changeBrightness(int val)
{
    int actualBrightness = getScreenActuallightMax();
    string path = backlightdevicepath  + "brightness";
    int fout = open(path.c_str(), O_WRONLY);
    int newBrightness = actualBrightness + val;
    char str[4];
    sprintf(str,"%d",newBrightness);
    write(fout, str, sizeof(str));
    close(fout);
}


int main(int argc, char const *argv[])
{
    int fd,c,res;
    struct termios oldtio,newtio;
    char buf[255];
    int maxBrightness = getScreenBacklightMax();
    int minBrightness = 150;

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
    int prevBrightness = getScreenActuallightMax();
    int i,sum,avgVal = 0,sensorVal,actualBrightness,change;
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
        actualBrightness = getScreenActuallightMax();
        // cout << avgVal <<"-" <<actualBrightness<<"\n";
        change = avgVal - prevBrightness;
        if(abs(change) > 20){
            if((actualBrightness + change) >= maxBrightness){
                changeBrightness(maxBrightness - actualBrightness);
            }
            else if ((actualBrightness + change) <= minBrightness)
            {
                changeBrightness(minBrightness - actualBrightness);
            }
            else{
                changeBrightness(change);
            }
            
        }
        prevBrightness = getScreenActuallightMax();
    }
    close(fd);
    return 0;
}
