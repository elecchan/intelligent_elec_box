#ifndef _HISTORY_OPT_H_HEADER  
#define _HISTORY_OPT_H_HEADER  

#define history_file "/etc/history"
#define TEMP_H_WARM  1
#define TEMP_L_WARM  2
#define HUMI_H_WARM  3
#define HUMI_L_WARM  4
#define DOOR_WARM    5
#define WATER_WARM   6

//保存每次存储时的时间秒数
struct history_Time {
    int temp_h_time;
    int temp_l_time;
    int humi_h_time;
    int humi_l_time;
    int door_time;
    int water_time;
};
struct history_Time history_time;

void history_opt_init(void);
void history_opt_update(unsigned char type,unsigned int second);

#endif