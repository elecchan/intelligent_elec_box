#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

//执行指令函数带返回
int cmd_opt_system_ret(const char * cmd, char *retmsg, int msg_len) {
	FILE * fp;
	int res = -1;
	if (cmd == NULL || retmsg == NULL || msg_len < 0){
		printf("Err: Fuc:system paramer invalid!\n");
		return 1;
	}
	if ((fp = popen(cmd, "rw") ) == NULL){
		perror("popen");
		printf("Err: Fuc:popen error: %s\n", strerror(errno));
		return 2;
	}
	else{
		memset(retmsg, 0, msg_len);
		while (fgets(retmsg, msg_len, fp));{
			//printf("Fuc:fgets buf is %s\n", retmsg);
		}
		if ((res = pclose(fp)) == -1){
			printf("Fuc:close popen file pointer fp error!\n");
			return 3;
		}
		retmsg[strlen(retmsg) - 1] = '\0';
		return 0;
	}
}

//不带返回
void cmd_opt_system(char *cmd) {
	system(cmd);
}