/*
 *配置与配置文件操作
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include "cmd_opt.h"
#include "conf_opt.h"
#include "uci_opt.h"

unsigned char debug = 0;
//初始化全部数据为0
void conf_opt_init(void) {
    memset(&conf_data,0,sizeof(struct conf_Data));
    memset(&conf_setting,0,sizeof(struct conf_Setting));
}
//0-read 1-write
//需要获取到配置的只有DC12V AC24V 继电器输出
//其他输入、风扇、加热控制只需写配置就行,这些外设值全部是根据传感器或是外部触发而设定的
void conf_opt_status_update(unsigned char rd_wr) {
    char retmsg[16];
    char cmd[64];
    memset(cmd,0,sizeof(cmd));
    if(rd_wr == 0) {
        if(uci_get("uci get show.switch.volout8",retmsg,sizeof(retmsg)) == 0) {
            conf_data.dc_12v_1 = atoi(retmsg);
            conf_data.dc_12v_2 = atoi(retmsg+2);
            conf_data.dc_12v_3 = atoi(retmsg+4);
            conf_data.dc_12v_4 = atoi(retmsg+6);
            conf_data.dc_12v_5 = atoi(retmsg+8);
            conf_data.dc_12v_6 = atoi(retmsg+10);
            conf_data.dc_12v_7 = atoi(retmsg+12);
            conf_data.dc_12v_8 = atoi(retmsg+14);
        }
        if(uci_get("uci get show.switch.volout2",retmsg,sizeof(retmsg)) == 0) {
            conf_data.ac_24v_1 = atoi(retmsg);
            conf_data.ac_24v_2 = atoi(retmsg+2);
        }  
        if(uci_get("uci get show.switch.switchout",retmsg,sizeof(retmsg)) == 0) {
            conf_data.switch_out = atoi(retmsg);
        } 
        if(debug == 1)
            printf("conf_opt_status_update rd,%d %d %d %d %d %d %d %d,%d %d,%d\n",conf_data.dc_12v_1, \
                conf_data.dc_12v_2,conf_data.dc_12v_3,conf_data.dc_12v_4,conf_data.dc_12v_5,conf_data.dc_12v_6, \
                conf_data.dc_12v_7,conf_data.dc_12v_8,conf_data.ac_24v_1,conf_data.ac_24v_2, \
                conf_data.switch_out);
    }else if(rd_wr == 1) {
        sprintf(cmd,"uci set show.sensor.temp=%d",conf_data.temp_set);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.sensor.hum=%d",conf_data.humi_set);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.switch.volout8=%d-%d-%d-%d-%d-%d-%d-%d",conf_data.dc_12v_1,conf_data.dc_12v_2, \
            conf_data.dc_12v_3,conf_data.dc_12v_4,conf_data.dc_12v_5,conf_data.dc_12v_6,conf_data.dc_12v_7,conf_data.dc_12v_8);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.switch.volout2=%d-%d",conf_data.ac_24v_1,conf_data.ac_24v_2);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.switch.switchout=%d",conf_data.switch_out);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.switch.switchin=%d",conf_data.switch_in);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.device.fan=%d",conf_data.fan);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.device.heat=%d",conf_data.heat);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.device.led=%d",conf_data.fan);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.device.door=%d",conf_data.box_open);
        system(cmd);

        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"uci set show.device.water=%d",conf_data.box_water);
        system(cmd);

        uci_commit();
    }
}
//系统设置配置更新
//暂时只需全部读取，无需写配置
void conf_opt_setting_update(unsigned char rd_wr) {
    char retmsg[16];
    if(rd_wr == 0) {
        if(uci_get("uci get setting.device.name",retmsg,sizeof(retmsg)) == 0) {
            strcpy(conf_setting.device_name,retmsg);
        }
        if(uci_get("uci get setting.device.org",retmsg,sizeof(retmsg)) == 0) {
            strcpy(conf_setting.organization,retmsg);
        }
        if(uci_get("uci get setting.device.type",retmsg,sizeof(retmsg)) == 0) {
            strcpy(conf_setting.type,retmsg);
        }
        if(uci_get("uci get setting.device.location",retmsg,sizeof(retmsg)) == 0) {
            strcpy(conf_setting.location,retmsg);
        }

        if(uci_get("uci get setting.server.ip",retmsg,sizeof(retmsg)) == 0) {
            strcpy(conf_setting.server_ip,retmsg);
        }
        if(uci_get("uci get setting.server.port",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.server_port = atoi(retmsg);
        }

        if(uci_get("uci get setting.uart.rs232_baud",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs232_baud = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs232_databit",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs232_databit = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs232_parity",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs232_parity = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs232_stopbit",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs232_stopbit = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs485_baud",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs485_baud = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs485_databit",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs485_databit = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs485_parity",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs485_parity = atoi(retmsg);
        }
        if(uci_get("uci get setting.uart.rs485_stopbit",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.rs485_stopbit = atoi(retmsg);
        }

        if(uci_get("uci get setting.warm.temperature_h",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.temperature_h = atoi(retmsg);
        }
        if(uci_get("uci get setting.warm.temperature_l",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.temperature_l = atoi(retmsg);
        }
        if(uci_get("uci get setting.warm.humidity_h",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.humidity_h = atoi(retmsg);
        }
        if(uci_get("uci get setting.warm.humidity_l",retmsg,sizeof(retmsg)) == 0) {
            conf_setting.humidity_l = atoi(retmsg);
        }
        if(debug)
            printf("%d %d %d %d\n",conf_setting.temperature_h,conf_setting.temperature_l,conf_setting.humidity_h,conf_setting.humidity_l);
    }
}