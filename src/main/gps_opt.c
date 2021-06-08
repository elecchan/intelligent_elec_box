#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <stdlib.h>
#include "gps_opt.h"
#include "dev_opt.h"

void *gps_opt_thread(void *argv) {
	printf("gps_opt_thread start\n");
	char gps_buf[80];//gprmc指令最大70字节
	unsigned char ret = -1;
	while(1) {
		sleep(1);
		if(dev_uart_recv(gps_opt.fd,gps_buf,sizeof(gps_buf)) > 10) {
			ret = gps_opt_gprmc(gps_buf,&gps_buf.gprmc);
		}
		if(ret == 0) {
			printf("=============  GPS globle location  ==============\n");
			printf("GPS status: %c  [A:useful V:useless]\n" ,gps_opt.gprmc.pos_state);
			printf("GPS modle : %c  [A:auto D:diff]\n" , gps_opt.gprmc.mode);
			printf("Date      : 20%02d-%02d-%02d\n", gps_opt.gprmc.date%100, (gps_opt.gprmc.date%10000)/100,gps_opt.gprmc.date/10000);
			printf("Time      : %02d:%02d:%02d\n",(gps_opt.gprmc.time/10000+8)%24,(gps_opt.gprmc.time%10000)/100,gps_opt.gprmc.time%100);
			printf("Latitude  : %.3fN\n",(gps_opt.gprmc.latitude/100));
			printf("Longitude : %.3fE\n",(gps_opt.gprmc.longitude/100));
			printf("Speed     : %.3fm/s\n",gps_opt.gprmc.speed);
			printf("==================================================\n");
		}
	}
	pthread_exit((void*)0);
	return NULL;
}

int gps_opt_init(void) {
	memset(&gps_opt,0,sizeof(struct gps_Opt));
	//B38400 8数据位 无校验 1停止位
	int fd = dev_uart_open(GPS_DEV,3,8,1,1);
	if(fd > 0)
		if (pthread_create(&gps_opt.pthread_gps, NULL, (void*)gps_opt_thread, NULL)) 
			return -1;
	gps_opt.fd = fd;
	return fd;
}

void gps_opt_deinit(void) {
	dev_uart_close(gps_opt.fd);
}

int gps_opt_gprmc (char *buff,struct GPRMC *gprmc)
{
	char *ptr = NULL;
    if(gprmc == NULL)
        return -1;
    if(strlen(buff) < 10)
        return -1;
    if(NULL == (ptr = strstr(buff,"$GPRMC")))//strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串
        return -1;
    memset(gprmc,0,sizeof(struct GPRMC));
    sscanf(ptr,"$GPRMC,%d.000,%c,%f,N,%f,E,%f,%f,%d,,,%c*",&(gprmc->time),&(gprmc->pos_state),&(gprmc->latitude), \
    	&(gprmc->longitude),&(gprmc->speed),&(gprmc->direction),&(gprmc->date),&(gprmc->mode));
	return 0;
}