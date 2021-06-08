#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_opt.h"
#include "conf_opt.h"
/*
 *检查文件是否存在
 */
unsigned char file_detect(char *file_name) {
	if((access(file_name,F_OK)) == -1) {
		return 0;
	}else {
		return 1;
	}
	return 0;
}

/*
 *删除文件
 */
void file_delete(char *file) {
	char cmd[64];
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",file);
	system(cmd);
}

/*
 *文件追加内容
 */
void file_add_content(char *file_path,char *content) {
	//使用命令行模式追加内容
	char cmd[128];
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo %s >> %s",content,file_path);
	system(cmd);
}

