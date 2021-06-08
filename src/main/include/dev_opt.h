#ifndef _DEV_OPT_H_HEADER  
#define _DEV_OPT_H_HEADER  
#include <termios.h>
/****************************串口操作函数*******************************/
//#define TTY_DEV "/dev/ttyS0"
int dev_uart_open(char *dev,int baud,int databit,int parity,int stopbit);
int dev_uart_close(int fd);
speed_t dev_uart_get_baudrate(int baudrate);
int dev_uart_config(int fd,int baud,int databit,int parity,int stopbit);
int dev_uart_send(int fd, char *data, int datalen);
int dev_uart_recv(int fd, char *data, int datalen);

struct  dev_Rs232
{
	int fd;
	int baudrate;
	int databit;
	int stopbit;
	int parity;
};
struct  dev_Rs485
{
	int fd;
	int baudrate;
	int databit;
	int stopbit;
	int parity;
};
struct dev_Uart
{
	struct dev_Rs232 dev_rs232;
	struct dev_Rs485 dev_rs485;
};
struct dev_Uart dev_uart;
/****************************gpio操作函数 通过命令行读写方式**********************************/
unsigned char dev_gpio_read(char *cmd);
void dev_gpio_write(char *cmd);
unsigned char dev_opt_status_update(unsigned char rd_wr);
unsigned char dev_opt_setting_update(unsigned char rd_wr);
void dev_opt_heat(unsigned char on_off);
void dev_opt_led(unsigned char on_off);
void dev_opt_fan(unsigned char on_off);
/****************************传感器读取接口********************************/
int dev_dht11_open(void);
void dev_dht11_close(void);
unsigned char dev_dht11_read(unsigned char * temperature,unsigned char * humirity);
/*******************************************************************/
void dev_opt_init(void);
void dev_opt_deinit(void);
#endif
