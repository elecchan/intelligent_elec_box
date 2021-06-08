#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "fournet_opt.h"

//检测某个文件/设备是否存在
int fournet_file_detect(char *file_name) {
	if((access(file_name,F_OK)) == -1) {
		return 0;
	}else {
		return 1;
	}
	return -1;
}

//检测某个进程是否运行中
int fournet_process_detect(char *process_name) {
	FILE* fp; 
	int count; 
	char buf[128]; 
	char command[64]; 
	sprintf(command, "ps | grep %s | grep -v grep | wc -l",process_name ); 
 
	if((fp = popen(command,"r")) == NULL) {
		printf("popen err\n");
		return -2;//打开管道失败 
	}
	if( (fgets(buf,128,fp))!= NULL ) 
	{
		//printf("fournet_process_detect buf=%s\n",buf);
		count = atoi(buf); 
		if(count  == 0) {
			//printf("process not found\n"); 
			pclose(fp); 
			return 0;
		}
		else {
			//printf("process num is:%d\n",count); 
			pclose(fp); 
			return 1;
		}
	} else
		printf("fournet_process_detect null\n");
	return 0; 
}

//获取设备ip
int fournet_ip_get(char *eth_name,char *ip) {
	char *temp = NULL; 
	int inet_sock; 
	struct ifreq ifr; 
	inet_sock = socket(AF_INET, SOCK_DGRAM, 0); 
	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name)); 
	memcpy(ifr.ifr_name, eth_name, strlen(eth_name)); 
	if(0 != ioctl(inet_sock, SIOCGIFADDR, &ifr)) { 
		perror("ioctl error"); 
		return -1; 
	} 
	temp = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr); 
	memcpy(ip, temp, strlen(temp)); 
	close(inet_sock); 
	return 0;
}
//4G模块的检测 如果没拨号则执行拨号
int fournet_detect_and_process(char *dev_name,char *process) {
	//检测到模块已经接入
	if(fournet_file_detect(dev_name) == 1) {
		//printf("%s detected\n",dev_name);
		//检测是否已经拨号过 
		if(fournet_process_detect(process) == 0) {
			printf("start %s\n",process);
			//没拨号过的则拨号
			system("fournet &");
		}
		return 1;
	}
	return 0;
}