
#ifndef _CONF_H_HEADER  
#define _CONF_H_HEADER  
 

struct conf_Data  
{  
	char temp_set;
	char temp_rel;
	char humi_set;
	char humi_rel;
	float light_set;
	float light_rel;
	unsigned char dc_12v_1;
	unsigned char dc_12v_2;
	unsigned char dc_12v_3;
	unsigned char dc_12v_4;
	unsigned char dc_12v_5;
	unsigned char dc_12v_6;
	unsigned char dc_12v_7;
	unsigned char dc_12v_8;
	unsigned char ac_24v_1;
	unsigned char ac_24v_2;
	unsigned char switch_out;
	unsigned char fan;
	unsigned char heat;
	unsigned char led;
	unsigned char switch_in;
	unsigned char box_open;
	unsigned char box_water;
	unsigned char update;//更新标志
};  
//全局配置信息结构体
struct conf_Data conf_data;

struct conf_Setting {
	char device_name[32];//设备名
	char organization[8];//所属组织
	char type[8];//设备类型
	char location[16];//地理位置 可以是经纬度
	char server_ip[16];//服务器ip地址
	int server_port;//服务器端口
	unsigned char rs232_baud;
	unsigned char rs232_databit;
	unsigned char rs232_stopbit;
	unsigned char rs232_parity;
	unsigned char rs485_baud;
	unsigned char rs485_databit;
	unsigned char rs485_stopbit;
	unsigned char rs485_parity;
	int temperature_h;
	int temperature_l;
	int humidity_h;
	int humidity_l;
};
struct conf_Setting conf_setting;
//
void conf_opt_init(void);
//0-read 1-write
void conf_opt_status_update(unsigned char rd_wr);
//系统设置配置更新
void conf_opt_setting_update(unsigned char rd_wr);

#endif