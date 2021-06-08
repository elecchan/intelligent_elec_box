#include <stdlib.h>  
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <string.h>  
#include <signal.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <pthread.h> 

//socket通讯协议
/*说明:
  
*/
int socket_client_handle();
void socket_opt_init(void);
void socket_opt_update(void);

struct socket_Opt {
  int fd;
  pthread_t pthread_main;
  pthread_t pthread_status;
  pthread_t pthread_heart;
  pthread_mutex_t pthread_mutex_lock;
  char server_ip[16];
  int server_port;
  char fournet_ip[20];
};

struct socket_Protocol
{
  unsigned char head[2];       //协议头"<@"
  unsigned char version[4];    //版本协议,0字节代表机型，1和2，表示版本
  unsigned char protocol[2];   //命令协议，分别有：
                               /*
                                A0 上传状态信息
                                {[设备编号][四字节信息  5*CHAR]}
                                B0 上传报警信息
                                
                                C0 服务器下发指令
                                T0 心跳协议
                               */
  unsigned int length;        //0 -255实际内容的长度，不包含协议头的长度
};

struct socket_Opt socket_opt;