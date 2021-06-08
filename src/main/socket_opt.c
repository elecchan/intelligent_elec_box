#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "socket_opt.h"
#include "fournet_opt.h"
#include "conf_opt.h"

void *socket_main_thread(void *argv);

void socket_opt_init(void) {
	memset(&socket_opt,0,sizeof(struct socket_Opt));
	socket_opt.server_port = conf_setting.server_port;
	strcpy(socket_opt.server_ip,conf_setting.server_ip);
	//pthread_mutex_init(&pthread_mutex_lock, NULL);
	socket_client_handle();
}

void socket_opt_update(void) {
	socket_opt.server_port = conf_setting.server_port;
	strcpy(socket_opt.server_ip,conf_setting.server_ip);
}

//创建线程
int socket_client_handle(void) {
	if (pthread_create(&socket_opt.pthread_main, NULL, (void*)socket_main_thread, NULL)) {
		printf("create socket pthread error!/n");
		return -1;
	}
	return 0;
}

//心跳帧线程
void *socket_heart_thread(void *argv) {
	printf("socket_heart_thread creat\n");
	char head_buf[32];
	char send_buf[64];
	memset(head_buf,0,sizeof(head_buf));
	struct socket_Protocol head;
	head.head[0] = '<';
	head.head[1] = '@';
	strcpy(head.version,"B001");
	head.protocol[0] = 'T';
	head.protocol[1] = '0';
	while(1) {
		sprintf(head_buf,"{[%s]}",conf_setting.device_name);
		head.length = htonl(strlen(head_buf));
		memcpy(send_buf,&head,sizeof(struct socket_Protocol));
		memcpy(send_buf + sizeof(struct socket_Protocol),head_buf,strlen(head_buf));
		socket_send(socket_opt.fd,send_buf,sizeof(struct socket_Protocol) + strlen(head_buf));
		sleep(30);
	}
	pthread_exit((void*)0);
	return NULL;
}
//上报设备状态信息、报警信息线程
void *socket_status_thread(void *argv) {
	printf("socket_status_thread creat\n");
	char head_buf[128];
	char send_buf[128];
	memset(head_buf,0,sizeof(head_buf));
	struct socket_Protocol sHead;
	struct socket_Protocol wHead;
	//设备状态头
	sHead.head[0] = '<';
	sHead.head[1] = '@';
	strcpy(sHead.version,"B001");
	sHead.protocol[0] = 'A';
	sHead.protocol[1] = '0';
	//报警信息头
	while(1) {
		//更新标志有更新就上报状态信息
		//S1温度，S2湿度，S3光照度，S4箱门，S5漏水，S6LED，S7开关输入，S8GPS，S98路电源，SA2路电源，
		//SB开关输出，SC风扇，SD加热棒
		if(conf_data.update == 1) {
			conf_data.update = 0;
			sprintf(head_buf,"{[%s][S1:%d,S2:%d,S3:%0.2f,S4:%d,S5:%d,S6:%d,S7:%d,S8:%0.2f-%0.2f,S9:%d%d%d%d%d%d%d%d,SA:%d%d,SB:%d,SC:%d,SD:%d]}", \
				conf_setting.device_name, \
				conf_data.temp_rel,conf_data.humi_rel,conf_data.light_rel,conf_data.box_open, \
				conf_data.box_water,conf_data.led,conf_data.switch_in,1.11,2.22,conf_data.dc_12v_1, \
				conf_data.dc_12v_2,conf_data.dc_12v_3,conf_data.dc_12v_4,conf_data.dc_12v_5, \
				conf_data.dc_12v_6,conf_data.dc_12v_7,conf_data.dc_12v_8,conf_data.ac_24v_1, \
				conf_data.ac_24v_2,conf_data.switch_out,conf_data.fan,conf_data.heat);
			sHead.length = htonl(strlen(head_buf));
			memcpy(send_buf,&sHead,sizeof(struct socket_Protocol));
			memcpy(send_buf + sizeof(struct socket_Protocol),head_buf,strlen(head_buf));
			socket_send(socket_opt.fd,send_buf,sizeof(struct socket_Protocol) + strlen(head_buf));
		}
		sleep(1);
	}
	pthread_exit((void*)0);
	return NULL;
}
//线程一旦开启便不停运行,如果读写过程中出错需要重新从connect开始,防止服务器主动断开
void *socket_main_thread(void *argv) {
	printf("socket_main_thread creat\n");
	struct sockaddr_in addr_serv;//服务器端地址
reconnect:
	socket_opt.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_opt.fd < 0) {
		return -1;
	}
connect:
	//检测4G模块是否拨号
	/*while(fournet_detect_and_process("/dev/qcqmi1","fournet") == 0) 
		sleep(5);
	//检查4G模块是否获取到IP
	while(fournet_ip_get("eth1",socket_opt.fournet_ip) == -1) {
		system("ifconfig eth1 down");
		system("ifconfig eth1 up");
		sleep(5);
	}*/
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port =  htons(socket_opt.server_port);
	addr_serv.sin_addr.s_addr = inet_addr(socket_opt.server_ip);
	if ( connect(socket_opt.fd, (struct sockaddr *)&addr_serv, sizeof(struct sockaddr)) < 0) {
		printf("connect err,ip:%s,port:%d\n",socket_opt.server_ip,socket_opt.server_port);
		//连接不上需要不断重连
		sleep(2);
		goto connect;
	}
	printf("main thread socket connect success\n");
	//创建心跳、上传状态线程
	pthread_create(&socket_opt.pthread_heart, NULL, (void*)socket_heart_thread, NULL);
	pthread_create(&socket_opt.pthread_status, NULL, (void*)socket_status_thread, NULL);
	while(1) {
		//不断处理解析收到的平台下发指令
		if(socket_opt_recv_and_parse(socket_opt.fd) < 0) {
			printf("socket_opt_recv_and_parse<0\n");
			//销毁相关线程
			pthread_cancel(socket_opt.pthread_heart);
			pthread_cancel(socket_opt.pthread_status);
			//防止4G掉线
			goto reconnect;
		} 
		sleep(1);
	}
	pthread_exit((void*)0);
	return NULL;
}
//
int socket_send(int fd,char *data,int length) {
	int rret = 0,ret = 0;
	if(fd > 0) {
		while(!(rret == length)) {
			ret = send(fd,data + rret,length - rret,0);
			rret = rret + ret;
			if(ret < 0) {
				close(fd);
				return -1;
			}
		}
	}else
		return -1;
	return 0;
}
//接收平台下发指令并解析应答
int socket_opt_recv_and_parse(int fd) {
	int i = 0,datalen = 0;
	char recvstr[128];
	struct socket_Protocol rhead;
	memset(recvstr,0,sizeof(recvstr));
	//接收数据头部信息
	i = recv(fd,recvstr,sizeof(struct socket_Protocol),0);
	printf("i=%d\n",i);
	if(i <= 0) {
		close(fd);
		return -1;
	}
	memcpy(&rhead,recvstr,sizeof(struct socket_Protocol));
	datalen = ntohl(rhead.length);
	memset(recvstr,0,sizeof(recvstr));
	//接收剩余数据
	if(datalen >0) {
		i = recv(fd,recvstr,datalen,0);
		if(i < 0) {
			close(fd);
			return 0;
		}
	}
	//协议头、版本对应不上返回
	if((rhead.head[0] != '<') || (rhead.head[1] != '@'))
		return 0;
	if((rhead.version[0] != 'B') || (rhead.version[1] != '0') || (rhead.version[2] != '0') || (rhead.version[3] != '1'))
		return 0;
	switch(rhead.protocol[0]) {
		case 'C'://平台下发控制指令
			printf("get platform data:%s\n",recvstr);
		break;
		case 'K'://应答指令
			printf("get platform ack!\n");
		break;
		default:
		break;
	}
	return 0;
}
