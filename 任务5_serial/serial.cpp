/**
 * @file serial.cpp
 * @author Sandwich (1293716870@qq.com)
 * @brief 
 * @date 2021-10-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

using namespace::std;

int main()
{
    int fd;
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
       cout << "Error Opening! " << endl;
    }
    else
    {
        cout << "Successful openning!" << endl;
    }
    struct termios SPSettings;
    tcgetattr(fd, &SPSettings);
    
    //设置波特率
    cfsetispeed(&SPSettings, B115200);
    cfsetospeed(&SPSettings, B115200);
    
   //设置没有校验
    SPSettings.c_cflag |= CLOCAL | CREAD;
    
    //设置停止位为１
   SPSettings.c_cflag &= ~CSTOPB;
   
   //设置数据位为8
    SPSettings.c_cflag &= ~CSIZE;
    SPSettings.c_cflag |= CS8;
    
    //关闭软件流动控制
    SPSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    
   //设置串口通信非规范模式    
    SPSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); 

    SPSettings.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &SPSettings) != 0)
    {
        cout << "Error Setting!" << endl;
    }
    else
    {
        cout << "Successful Setting!" << endl;
    }
    char transmission[] = "Sandwich can do it";
    

    int identify;
    identify=write(fd, buffer, strlen(buffer));
    if (identify < 0)
    {
        cout << "Error writting!" << endl;
    }
    else
    {
        cout << "Successful writting!" << endl;
    }

    close(fd);
    return 0;
}
