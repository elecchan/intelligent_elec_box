#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>//close()
#include<netinet/in.h>//struct sockaddr_in
#include<arpa/inet.h>//inet_ntoa
#include <pthread.h>

#define DEST_PORT 55532
#define DEST_IP_ADDRESS "192.168.0.159"

/*客户端的处理过程*/
void process_info(int s)
{
  int send_num;
  int recv_num;
  char send_buf[50];
  char recv_buf[1024];
  while (1) {
    printf("begin send\n");
    sprintf(send_buf, "get_wifi_info");
    send_num = send(s, send_buf, sizeof(send_buf), 0);
    if (send_num < 0) {
      perror("send");
      exit(1);
    } else {
      printf("send sucess:%s\n", send_buf);
    }
    printf("begin recv:\n");
    recv_num = recv(s, recv_buf, sizeof(recv_buf), 0);
    if (recv_num < 0) {
      perror("recv");
      exit(1);
    } else {
      printf("recv sucess:%d %s\n", strlen(recv_buf), recv_buf);
      sprintf(send_buf, "disconnect");
      send(s, send_buf, sizeof(send_buf), 0);
      close(s);
      break;
    }
  }
}
////////////////////////////////////////////////////////////////////
struct socketData {
  char type;//协议类型
  char length;//数据长度<=255
  char data[255];
};
/*
type=0xB3,length=n,data="onvif/serial/cable_tester/cable_search/cable_tdr" ==>
                <== type=0xA4,length=n,data="data"
*/
void process_info2(int s) {
  struct socketData rdbuf;
  while (1) {
    if (recv(s, &rdbuf, 277, 0) > 0) {
      printf("recv sucess: type = 0x%x,len=%d,data=%s\n", rdbuf.type, rdbuf.length, rdbuf.data);
    } else {
      perror("recv");
      exit(1);
    }
  }
}
void *write_thread(void *argv) {
  struct socketData wrbuf;
  int *fd;
  fd = (int *)argv;
  memset(&wrbuf, 0, sizeof(struct socketData));
  wrbuf.type = 0x14;
  strcpy(wrbuf.data, "onvif");
  wrbuf.length = strlen(wrbuf.data);
  while (1) {
    send(*fd, &wrbuf, strlen(&wrbuf), 0);
    sleep(1);
  }
}
int main(int argc, char *argv[])
{
  pthread_t write_id;
  int sock_fd;
  struct sockaddr_in addr_serv;//服务器端地址

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("sock");
    exit(1);
  } else {
    printf("sock sucessful:\n");
  }
  memset(&addr_serv, 0, sizeof(addr_serv));
  addr_serv.sin_family = AF_INET;
  addr_serv.sin_port =  htons(DEST_PORT);
  addr_serv.sin_addr.s_addr = inet_addr(DEST_IP_ADDRESS);
  if ( connect(sock_fd, (struct sockaddr *)&addr_serv, sizeof(struct sockaddr)) < 0) {
    perror("connect");
    printf("connect (%d)\n", errno);
    exit(1);
  } else {
    printf("connect sucessful\n");
  }

  if (pthread_create(&write_id, NULL, (void*)write_thread, (int *)&sock_fd)) {
    printf("Create pthread error!/n");
    return 1;
  }
  process_info2(sock_fd);
  close(sock_fd);
}
