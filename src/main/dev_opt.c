/*
外部硬设所有操作接口
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/shm.h>

#include "shmdata.h"
#include "conf_opt.h"
#include "dev_opt.h"
#include "cmd_opt.h"


/********************************串口相关操作函数************************************************/
int dev_uart_open(char *dev,int baud,int databit,int parity,int stopbit) {
	int fd;
	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd < 0)
		return -1;
	dev_uart_config(fd,baud,databit,parity,stopbit);
	return fd;
}

int dev_uart_close(int fd) {
	if(fd > 0)
        close(fd);
    return 0;
}

speed_t dev_uart_getbaudrate(int baudrate) {
	switch(baudrate) {
		case 1:return B115200;
		case 2:return B57600;
		case 3:return B38400;
		case 4:return B19200;
		case 5:return B9600;
		case 6:return B4800;
		case 7:return B2400;
		default:return -1;
	}
}

int dev_uart_config(int fd,int baud,int databit,int parity,int stopbit) {
    if(fd < 0)
        return -1;
	speed_t speed;  
	struct termios cfg;
	struct termios options;
	printf("fd:%d baud:%d %d %d %d\n",fd,baud,databit,parity,stopbit);
	speed = dev_uart_getbaudrate(baud);
	if(speed == -1)
		return -2;
	
	tcgetattr(fd, &options);
	
	//cfsetispeed(&cfg, speed);
    //cfsetospeed(&cfg, speed);
	
	//修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;
	//不使用流控制
	options.c_cflag &= ~CRTSCTS;
	//设置数据位
    options.c_cflag &= ~CSIZE; //屏蔽其他标志位
    switch (databit) {
		case 5    :
        options.c_cflag |= CS5;
        break;
		case 6    :
        options.c_cflag |= CS6;
        break;
		case 7    :
        options.c_cflag |= CS7;
        break;
		case 8:
        options.c_cflag |= CS8;
        break;
		default:
        return -4;
    }
	//设置校验位
    switch (parity) {
		case 1://无校验
		options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
		break;
		case 2://Even-偶数校验
		options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
		break;
		case 3://Odd-奇数校验
		options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
		default:break;
	}
	// 设置停止位
    switch (stopbit) {
		case 1://1个停止位
		options.c_cflag &= ~CSTOPB;
        break;
		case 2://2个停止位
		options.c_cflag |= CSTOPB;
        break;
	}
	//修改输出模式，原始数据输出
    //options.c_oflag &= ~OPOST;
    //设置等待时间和最小接收字符
    //options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
    //options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */
	//如果发生数据溢出，接收数据，但是不再读取
	options.c_oflag = 0; //输出模式  
    options.c_lflag = 0; //不激活终端模式
	cfsetospeed(&options, speed);//设置波特率 
    tcflush(fd,TCIFLUSH);
	//激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd,TCSANOW,&options) != 0)
        return -5;
    return 0;
}

int dev_uart_send(int fd, char *data, int datalen)  
{  
    int len = 0;  
    len = write(fd, data, datalen);//实际写入的长度  
    if(len == datalen) {  
        return len;  
    } else {  
        tcflush(fd, TCOFLUSH);//TCOFLUSH刷新写入的数据但不传送  
        return -1;  
    }       
    return 0;  
}  

int dev_uart_recv(int fd, char *data, int datalen)  
{  
    int len=0;       
    unsigned char buf[datalen] = {0};
    if(read(fd, data, datalen) > 0) {
        usleep(20*1000);
        if(read(fd, buf, datalen) > 0)
            strcat(data,buf);
    }
    return strlen(data);  
}  
/*************************************gpio操作*************************************/
unsigned char dev_gpio_read(char *cmd) {
    char retmsg[8];
    if(cmd_opt_system_ret(cmd, retmsg, sizeof(retmsg)) < 0) {
        printf("dev_gpio_read err\n");
        return 0;
    }
    if(!strcmp(retmsg,"1"))
        return 1;
    return 0;
}

void dev_gpio_write(char *cmd) {
    cmd_opt_system(cmd);
}

//读、写外设状态
unsigned char dev_opt_status_update(unsigned char rd_wr) {
    unsigned char status = 0;
    unsigned char changed = 0;
    char cmd[64];
    if(rd_wr == 0) {
        //读取温湿度比对
        //dev_dht11_read(&conf_data.temp_rel,&conf_data.humi_rel);
        if((conf_data.temp_rel != conf_data.temp_set) || (conf_data.humi_rel != conf_data.humi_set)) {
            conf_data.temp_set = conf_data.temp_rel;
            conf_data.humi_set = conf_data.humi_rel;
            changed = 1;
        }
        //1路光耦输入，低电平有效
        status = dev_gpio_read("cat /sys/class/gpio/gpio9/value");
        //printf("1.status=%d\n",status);
        if(status == conf_data.switch_in) {
            conf_data.switch_in = status^1;
            changed = 1;
        }
        //1路箱门开启检测，低电平有效
        status = dev_gpio_read("cat /sys/class/gpio/gpio3/value");
        //printf("2.status=%d\n",status);
        if(status == conf_data.box_open) {
            conf_data.box_open = status^1;
            changed = 1;
        }
        //1路箱门漏水检测，低电平有效
        status = dev_gpio_read("cat /sys/class/gpio/gpio4/value");
        //printf("3.status=%d\n",status);
        if(status == conf_data.box_water) {
            conf_data.box_water = status^1;
            changed = 1;
        }
    }else if(rd_wr == 1) {
        //8路DC12V输出，高电平使能
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio113/value",conf_data.dc_12v_1);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio114/value",conf_data.dc_12v_2);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio115/value",conf_data.dc_12v_3);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio116/value",conf_data.dc_12v_4);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio117/value",conf_data.dc_12v_5);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio118/value",conf_data.dc_12v_6);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio119/value",conf_data.dc_12v_7);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio120/value",conf_data.dc_12v_8);
        system(cmd);
        //2路AC12V输出，低电平使能
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio122/value",(conf_data.ac_24v_1 == 1)?0:1);
        system(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio124/value",(conf_data.ac_24v_2 == 1)?0:1);
        system(cmd);
        //1路继电器输出，低电平使能
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"echo %d > /sys/class/gpio/gpio121/value",(conf_data.switch_out == 1)?0:1);
        system(cmd);
    }
    return changed;
}
//更新系统设置设备 uart、服务器配置
unsigned char dev_opt_setting_update(unsigned char rd_wr) {
    if(rd_wr == 1) {
        //更新串口配置
        if((dev_uart.dev_rs232.baudrate != conf_setting.rs232_baud) || (dev_uart.dev_rs232.databit != conf_setting.rs232_databit) || (dev_uart.dev_rs232.parity != conf_setting.rs232_parity) || (dev_uart.dev_rs232.stopbit != conf_setting.rs232_stopbit)) {
            dev_uart.dev_rs232.baudrate = conf_setting.rs232_baud;
            dev_uart.dev_rs232.databit= conf_setting.rs232_databit;
            dev_uart.dev_rs232.parity = conf_setting.rs232_parity;
            dev_uart.dev_rs232.stopbit = conf_setting.rs232_stopbit;
            dev_uart_config(dev_uart.dev_rs232.fd,dev_uart.dev_rs232.baudrate,dev_uart.dev_rs232.databit,dev_uart.dev_rs232.parity,dev_uart.dev_rs232.stopbit);
        }
        if((dev_uart.dev_rs485.baudrate != conf_setting.rs485_baud) || (dev_uart.dev_rs485.databit != conf_setting.rs485_databit) || (dev_uart.dev_rs485.parity != conf_setting.rs485_parity) || (dev_uart.dev_rs485.stopbit != conf_setting.rs485_stopbit)) {
            dev_uart.dev_rs485.baudrate = conf_setting.rs485_baud;
            dev_uart.dev_rs485.databit = conf_setting.rs485_databit;
            dev_uart.dev_rs485.parity = conf_setting.rs485_parity;
            dev_uart.dev_rs485.stopbit = conf_setting.rs485_stopbit;
            dev_uart_config(dev_uart.dev_rs485.fd,dev_uart.dev_rs485.baudrate,dev_uart.dev_rs485.databit,dev_uart.dev_rs485.parity,dev_uart.dev_rs485.stopbit);
        }
    }
    return 0;
}
//加热棒操作
void dev_opt_heat(unsigned char on_off) {
    if(on_off == 0) {
        system("echo 1 > /sys/class/gpio/gpio123/value");
    }else if(on_off == 1) {
        system("echo 0 > /sys/class/gpio/gpio123/value");
    }
}
//led灯带操作
void dev_opt_led(unsigned char on_off) {
    if(on_off == 0) {
        system("echo 0 > /sys/class/gpio/gpio5/value");
    }else if(on_off == 1) {
        system("echo 1 > /sys/class/gpio/gpio5/value");
    }
}
//风扇操作
void dev_opt_fan(unsigned char on_off) {
    if(on_off == 0) {
        system("echo 0 > /sys/class/gpio/gpio8/value");
    }else if(on_off == 1) {
        system("echo 1 > /sys/class/gpio/gpio8/value");
    }
}
/*************************************传感器读取接口*************************************/
int dht11_fd = 0;
int dev_dht11_open(void) {
    dht11_fd = open("/dev/dht11", O_RDONLY);
    if(dht11_fd == -1) {
        printf("open failed!\n");
        return -1;
    }
    return dht11_fd;
}
void dev_dht11_close(void) {
    close(dht11_fd);
}
unsigned char dev_dht11_read(unsigned char * temperature,unsigned char * humirity) {
    unsigned char buf[6];
    int length;
    length = read(dht11_fd, buf, 6);
    if(length == -1) {
            printf("read error!\n");
            return 1;
    }
    *temperature = buf[2];
    *humirity = buf[0];
    //printf("temp=%d,hum=%d\n",*temperature,*humirity);
    return 0;
}

//
void dev_opt_init(void) {
    memset(&dev_uart,0,sizeof(struct dev_Uart));
    dev_dht11_open();//温湿度传感器初始化
    //串口初始化
    dev_uart.dev_rs232.baudrate = conf_setting.rs232_baud;
    dev_uart.dev_rs232.databit= conf_setting.rs232_databit;
    dev_uart.dev_rs232.parity = conf_setting.rs232_parity;
    dev_uart.dev_rs232.stopbit = conf_setting.rs232_stopbit;
    dev_uart.dev_rs232.fd = dev_uart_open("/dev/ttymxc2",dev_uart.dev_rs232.baudrate,dev_uart.dev_rs232.databit,dev_uart.dev_rs232.parity,dev_uart.dev_rs232.stopbit);
    dev_uart.dev_rs485.baudrate = conf_setting.rs485_baud;
    dev_uart.dev_rs485.databit = conf_setting.rs485_databit;
    dev_uart.dev_rs485.parity = conf_setting.rs485_parity;
    dev_uart.dev_rs485.stopbit = conf_setting.rs485_stopbit;
    dev_uart.dev_rs485.fd = dev_uart_open("/dev/ttymxc3",dev_uart.dev_rs485.baudrate,dev_uart.dev_rs485.databit,dev_uart.dev_rs485.parity,dev_uart.dev_rs485.stopbit);
}

void dev_opt_deinit(void) {
    dev_dht11_close();
    dev_uart_close(dev_uart.dev_rs232.fd);
    dev_uart_close(dev_uart.dev_rs485.fd);
}
