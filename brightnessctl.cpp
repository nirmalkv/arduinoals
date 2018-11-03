#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

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
    changeBrightness(atoi(argv[1]));
    cout<<getScreenBacklightMax()<<endl;
    cout<<getScreenActuallightMax()<<endl;
    return 0;
}
