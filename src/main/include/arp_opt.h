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

#define MAX_LAN 16
//arp包
struct arp_packet
{
    //DLC Header
    //接收方mac
    unsigned char mac_target[ETH_ALEN];
    //发送方mac
    unsigned char mac_source[ETH_ALEN];
    //Ethertype - 0x0806是ARP帧的类型值
    unsigned short ethertype;

    //ARP Frame
    //硬件类型 - 以太网类型值0x1
    unsigned short hw_type;
    //上层协议类型 - IP协议(0x0800)
    unsigned short proto_type;
    //MAC地址长度
    unsigned char mac_addr_len;
    //IP地址长度
    unsigned char ip_addr_len;
    //操作码 - 0x1表示ARP请求包,0x2表示应答包
    unsigned short operation_code;
    //发送方mac
    unsigned char mac_sender[ETH_ALEN];
    //发送方ip
    unsigned char ip_sender[4];
    //接收方mac
    unsigned char mac_receiver[ETH_ALEN];
    //接收方ip
    unsigned char ip_receiver[4];
    //填充数据
    unsigned char padding[18];
};
//存放收到的arp队列 存放最新3个获取到arp网段
struct arp_list {
	int arp_count;
	char arp_list_1[16];
	char arp_list_2[16];
	char arp_list_3[16];
};
//存放通过arp扫描到的新增网段信息
struct br_lan_add_list {
    char ifname[16];
    char ip[16];
};
//打印arp包
void print_arp_packet(struct arp_packet ap);
//创建arp socket
int arp_socket(void);
//arp绑定
int arp_bind(int fd,char *dev);
//接收arp包
int arp_recv(int fd);
//关闭arp socket
int arp_close(int fd);
//建立arp连接
int arp_setup(char *dev);
//解析收到arp数据并排列最新队列
//返回：-1 无效数据
//       0 最近3次数据都一致
//       1 最近三次数据不一致
int arp_recv_and_list(int fd);
//得到arp并跟列表的网段做比较 如果是新的网段那么添加新网段，如果网段数超过暂定数目，则
//不断更新列表
int arp_recv_and_add_new_lan(int fd);
//查询网段是否在列表内
int arp_lan_in_list(char *arp_ip,int br_lan_count);
//将新网段添加到列表内
int add_new_lan_list(char *arp_ip,int br_lan_count);

///////////////////////////////////////
extern struct arp_list arpList;
//暂定最多存放16个跨网段
extern struct br_lan_add_list brLanAddList[MAX_LAN];
extern int br_lan_count;
