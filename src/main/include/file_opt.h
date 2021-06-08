#ifndef _FILE_OPT_H_HEADER  
#define _FILE_OPT_H_HEADER  

#define history_file "/etc/history"
#define TEMP_H_WARM  1
#define TEMP_L_WARM  2
#define HUMI_H_WARM  3
#define HUMI_L_WARM  4
#define DOOR_WARM    5
#define WATER_WARM   6

unsigned char file_detect(char *file_name);
void file_delete(char *file);
void file_add_content(char *file_path,char *content);

#endif