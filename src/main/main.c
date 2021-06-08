#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <pthread.h>
#include "socket_opt.h"
#include "shm_opt.h"
#include "shmdata.h"
#include "conf_opt.h"
#include "dev_opt.h"
#include "file_opt.h"
#include "history_opt.h"
#include "gps_opt.h"

#define READ  0
#define WRITE 1
#define ON    1
#define OFF   0

pthread_t pthread_main;
void *pthread_main_handle(void *argv) {
	printf("create main pthread\n");
	while(1) {
		//不断读取外部输入IO状态变化 如果有变化更新配置文件
		if(dev_opt_status_update(READ) == 1) {
			printf("dev update conf\n");
			conf_data.update = 1;
			conf_opt_status_update(WRITE);
		}
		//如果箱门打开、漏水
		if(conf_data.box_open == 1)
			history_opt_update(DOOR_WARM,5*60);
		if(conf_data.box_water == 1)
			history_opt_update(WATER_WARM,5*60);
		sleep(1);
	}
	pthread_exit((void*)0);
	return NULL;
}
/*传感器读取线程 无需频繁读取
数据比对并根据设置控制外设 并把报警类型写到记录文件
1.箱门开启如果光照低开启led
2.温度过高开启风扇，过低开启加热棒
*/

pthread_t pthread_sensor;
void *pthread_sensor_handle(void *argv) {
	unsigned char update = 0;
	while(1) {
		dev_dht11_read(&conf_data.temp_rel,&conf_data.humi_rel);
		//温度判断并更新配置
		if((conf_data.temp_rel > conf_setting.temperature_h) && (conf_data.fan != 1)) {
			dev_opt_fan(ON);
			dev_opt_heat(OFF);
			conf_data.fan = 1;
			conf_data.heat = 0;
			update = 1;
			history_opt_update(TEMP_H_WARM,5*60);
		}else if((conf_data.temp_rel < conf_setting.temperature_l) && (conf_data.heat != 1)) {
			dev_opt_heat(ON);
			dev_opt_fan(OFF);
			conf_data.fan = 0;
			conf_data.heat = 1;
			update = 1;
			history_opt_update(TEMP_L_WARM,5*60);
		}else if((conf_data.fan == 1) || (conf_data.heat == 1)) {
			dev_opt_heat(OFF);
			dev_opt_fan(OFF);
			conf_data.fan = 0;
			conf_data.heat = 0;
			update = 1;
		}
		//湿度
		if(conf_data.humi_rel > conf_setting.humidity_h) {
			history_opt_update(HUMI_H_WARM,5*60);
			update = 1;
		}else if(conf_data.humi_rel < conf_setting.humidity_l) {
			history_opt_update(HUMI_L_WARM,5*60);
			update = 1;
		}
		if(update == 1) {
			update = 0;
			conf_opt_status_update(WRITE);
		}
		sleep(5);
	}
	pthread_exit((void*)0);
	return NULL;
} 

int main(void) {
	//test
	//char date[64];
	//cmd_opt_system_ret("date +\"%Y-%m-%d %H:%M.%S\"\n",date,sizeof(date));
	//printf("date:%sn",date);
	//1.获取配置并初始化设备
	conf_opt_init();
	history_opt_init();
	//获取配置文件数据,根据配置文件数据更新外设状态
	conf_opt_status_update(READ);
	dev_opt_status_update(WRITE);
	//读取系统设置,并初始化外设
	conf_opt_setting_update(READ);
	//dev_opt_setting_update(WRITE);

	//2.初始化硬件设备
	dev_opt_init();
	gps_opt_init();
	//3.共享内存
	creat_shm();
	//4.开辟新线程
	if (pthread_create(&pthread_main, NULL, (void*)pthread_main_handle, NULL)) {
		printf("create main pthread error!/n");
		return -1;
	}
	if (pthread_create(&pthread_sensor, NULL, (void*)pthread_sensor_handle, NULL)) {
		printf("create sensor pthread error!/n");
		return -1;
	}
	socket_opt_init();
	//5.不断读取共享内存是否更新数据,如果更新了则获取配置文件数据
	while(1) {
		if(intelligent_conf->useful == 1) {
			printf("get the share data\n");
			intelligent_conf->useful = 0;
			conf_data.update = 1;
			//读取配置文件,根据配置文件设置值驱动外设
			conf_opt_status_update(READ);
			dev_opt_status_update(WRITE);
			//读取系统设置配置，并根据配置更新外设
			conf_opt_setting_update(READ);
			socket_opt_update();
			dev_opt_setting_update(WRITE);
			socket_opt_update();
		}
		sleep(1);
	}
	delete_shm();
	dev_opt_deinit();
	gps_opt_deinit();
}