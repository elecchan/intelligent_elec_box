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

void get_br_lan_ip(char *br_ip) {
	struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;
	unsigned long ipv4[5];

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct!=NULL) {      
		if(!strcmp(ifAddrStruct->ifa_name,"br-lan")) {
			if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
           	 	// is a valid IP4 Address
           	 	tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
           	 	char addressBuffer[INET_ADDRSTRLEN];
            	inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            	//printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer); 
				//translate to 4 long
				ipv4[0] = htonl(inet_addr(addressBuffer));
				ipv4[4] = ipv4[0] % 256; 
				ipv4[3] = ipv4[0] / 256 % 256;
				ipv4[2] = ipv4[0] / 256 / 256 % 256;
				ipv4[1] = ipv4[0] / 256 / 256 / 256 % 256;
				printf("get_br_lan_ip:IP Address %d.%d.%d.%d   %d\n",ipv4[1],ipv4[2],ipv4[3],ipv4[4],ipv4[0]); 
				break;
        	} 
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
    }
    sprintf(br_ip,"%d.%d.%d",ipv4[1],ipv4[2],ipv4[3]);
}
//重启网络服务
int restart_network(void) {
	system("/etc/init.d/network restart");
}
