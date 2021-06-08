#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

//4G设备检测 传递设备节点
int fournet_file_detect(char *file_name);
//获取4G IP 传递网络设备名
int fournet_ip_get(char *eth_name,char *ip);
//检测是否已经拨号 传递要查找的进程名
int fournet_process_detect(char *process_name);
//4G模块的检测 如果没拨号则执行拨号
int fournet_detect_and_process(char *dev_name,char *process);