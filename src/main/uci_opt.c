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
#include "uci_opt.h"

//将网口切换为lan口
void uci_set_lan(void) {
	char cmd[64];
	//lan口wan口调转
	sprintf(cmd,"uci set network.lan.ifname=eth0.1");
	system(cmd);
	sprintf(cmd,"uci set network.wan.ifname=eth0.2");
	system(cmd);
	sprintf(cmd,"uci set network.wan.proto=dhcp");
	system(cmd);
	uci_commit();
}
//修改br-lan ip
void uci_set_lan_ip(char *ip) {
	char cmd[64];
	sprintf(cmd,"uci set network.lan.ipaddr=%s",ip);
	//printf("uci_set_lan_ip:%s\n",ip);
	system(cmd);
	uci_commit();
}
//将网口切换为wan口
void uci_set_wan(void) {
	char cmd[64];
	//lan口wan口调转
	sprintf(cmd,"uci set network.lan.ifname=eth0.2");
	system(cmd);
	sprintf(cmd,"uci set network.wan.ifname=eth0.1");
	system(cmd);
	sprintf(cmd,"uci set network.wan.proto=dhcp");
	system(cmd);
	uci_commit();
}
//将网口切换为wan口并拨号
void uci_set_wan_pppoe(char *user,char *passwd) {
	char cmd[64];
	//lan口wan口调转
	sprintf(cmd,"uci set network.lan.ifname=eth0.2");
	system(cmd);
	sprintf(cmd,"uci set network.wan.ifname=eth0.1");
	system(cmd);
	//修改接口类型为pppoe 设置用户名、密码
	sprintf(cmd,"uci set network.wan.proto=pppoe");
	system(cmd);
	sprintf(cmd,"uci set network.wan.username=%s",user);
	system(cmd);
	sprintf(cmd,"uci set network.wan.password=%s",passwd);
	system(cmd);
	uci_commit();
}
//获取网口接口类型 -1-err 1-lan 2-wan
int uci_get_port_type(void) {
	char retMsg[64];
	if(uci_get("uci get network.lan.ifname",retMsg,sizeof(retMsg)) == 0) {
		if(!strcmp(retMsg,"eth0.1"))
			return PORT_TYPE_LAN;//lan
		else 
			return PORT_TYPE_WAN;//wan
	}
	return -1;
}
//获取pppoe账号密码
int uci_get_pppoe_mess(char *user,char *passwd) {
	char retMsg[64];
	if(uci_get("uci get network.wan.username",retMsg,sizeof(retMsg)) == 0) {
		strcpy(user,retMsg);
		if(uci_get("uci get network.wan.password",retMsg,sizeof(retMsg)) == 0) {
			strcpy(passwd,retMsg);
			return 1;
		}
	}
	return -1;
}
//uci返回执行结果
int uci_get(const char * cmd, char *retmsg, int msg_len)
{
    FILE * fp;
    int res = -1;
    if (cmd == NULL || retmsg == NULL || msg_len < 0) {
        printf("Err: Fuc:system paramer invalid!\n");
        return 1;
    }
    if ((fp = popen(cmd, "rw") ) == NULL) {
        perror("popen");
        printf("Err: Fuc:popen error\n");
        return 2;
    } else{
        memset(retmsg, 0, msg_len);
        fread(retmsg, msg_len, 1,fp);
        if((res = pclose(fp)) == -1) {
            printf("Fuc:close popen file pointer fp error!\n");
            return 3;
        }        
        retmsg[strlen(retmsg)-1] = '\0';
        //printf("uci_get string:%s\n",retmsg);
        return 0;
    }
    return -1;
}
//提交修改信息
void uci_commit(void) {
	system("uci commit");
}
