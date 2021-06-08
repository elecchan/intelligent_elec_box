#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "conf_opt.h"
#include "file_opt.h"
#include "history_opt.h"

void history_opt_init(void) {
    memset(&history_time,0,sizeof(struct history_Time));
}
//记录文件检测 如果超过一定大小、记录条数、清除记录

/*
生成一条历史记录并写文件 可设置超时时间
格式：日期(2019-1-1:18:00:00)+报警类型(Temperature too low)+数值(-10)
 */
void history_opt_update(unsigned char type,unsigned int second) {
    //printf("history_opt_update,type=%d\n",type);
    char date[24];
    char content[128];
    memset(date,0,sizeof(date));
    memset(content,0,sizeof(content));
    cmd_opt_system_ret("date +\"%Y-%m-%d %H:%M:%S\"",date,sizeof(date));
    //获取系统时间
    struct timeval tv;
    struct timezone tz;
    gettimeofday (&tv , &tz);
    //组成数据内容
    switch(type) {
        case TEMP_H_WARM:
        if((tv.tv_sec - history_time.temp_h_time) > second ) {
            history_time.temp_h_time = tv.tv_sec;
            sprintf(content,"%s ------------------------------- Temperature is too high:%dC",date,conf_data.temp_rel);
            file_add_content(history_file,content);
        }
        break;
        case TEMP_L_WARM:
        if((tv.tv_sec - history_time.temp_l_time) > second ) {
            history_time.temp_l_time = tv.tv_sec;
            sprintf(content,"%s ------------------------------- Temperature is too low:%dC",date,conf_data.temp_rel);
            file_add_content(history_file,content);
        }
        break;
        case HUMI_H_WARM:
        if((tv.tv_sec - history_time.humi_h_time) > second ) {
            history_time.humi_h_time = tv.tv_sec;
            sprintf(content,"%s ------------------------------- Humirity is too high:%d%",date,conf_data.humi_rel);
            file_add_content(history_file,content);
        }
        break;
        case HUMI_L_WARM:
        if((tv.tv_sec - history_time.humi_l_time) > second ) {
            history_time.humi_l_time = tv.tv_sec;
            sprintf(content,"%s ------------------------------- Humirity is too low:%d%",date,conf_data.humi_rel);
            file_add_content(history_file,content);
        }
        break;
        case DOOR_WARM:
        if((tv.tv_sec - history_time.door_time) > second ) {
            history_time.door_time = tv.tv_sec;
            sprintf(content,"%s ------------------------------- Door is open",date);
            file_add_content(history_file,content);
        }
        break;
        case WATER_WARM:
        if((tv.tv_sec - history_time.water_time) > second ) {
            history_time.water_time = tv.tv_sec;
            sprintf(content,"%s ------------------------------- The water in the oven",date);
            file_add_content(history_file,content);
        }
        break;
    }
}