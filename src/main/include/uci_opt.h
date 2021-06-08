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

#define PORT_TYPE_LAN 1
#define PORT_TYPE_WAN 2

//将网口切换为lan口
void uci_set_lan(void);
//设置br-lan口ip
void uci_set_lan_ip(char *ip);
//将网口切换为wan口
void uci_set_wan(void);
//将网口切换为wan口并拨号
void uci_set_wan_pppoe(char *user,char *passwd);
//获取网口接口类型 1-lan 2-wan
int uci_get_port_type(void);
//获取pppoe账号密码
int uci_get_pppoe_mess(char *user,char *passwd);
//uci返回执行结果
int uci_get(const char * cmd, char *retmsg, int msg_len);
//提交修改
void uci_commit(void);