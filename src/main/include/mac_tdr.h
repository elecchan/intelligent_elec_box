
#ifndef _TDR_H
#define _TDR_H
/*
在线时：
长度显示---，衰减---，其他的按读数显示

开路、短路时：
长度按读数显示，线长大于10米才有衰减数值否则显示---，其他显示---
*/
typedef	struct _TDRresult		
{				//[0]~[4]对应线对1~4	
	unsigned char sta[4];	//线对状态 0:在线; 1:开路; 2:短路。路对状态为“在线”时，长度不可用，显示“---”	
	float length[4];		//线对长度 单位:m	
	float atten[4];			//衰减 单位:dB/100m (线长大于10米才有衰减数值) 
							// >-8.2dB-->good cable!   
							//>-11.8dB-->poor quality cable!
							//else-->wet cable!

	float ref[4];			//反射率，单位:%。当sta[]=0时有效
	float imp[4];			//阻抗，单位:ohm。当sta[]=0时有效
}TDRresult;
/*
length取一位小数
atten取一位小数
ref取一位小数
imp取整数部分
注：取小数时要四舍五入
*/

typedef struct userspace_i2c_info_s {
    unsigned char     bus_num;
    unsigned short    slave_addr;
    unsigned int      reg_addr;       //i2c device register address
    unsigned int      reg_size;       //register address size
    unsigned char *      val;
    unsigned int      val_size;       //register value size
    unsigned int      speed;          //100000 == 100KHz
} userspace_i2c_info_t;

typedef struct userspace_mdio_info_s {
    unsigned short    slave_addr;
    unsigned char      reg_addr;       //i2c device register address
    unsigned int *     val;
} userspace_mdio_info_t;


typedef struct
{
	unsigned int (* GetSystemTimeUS)(void);
	void (* delayms )( unsigned int MsDly );/* 毫秒级别的延时函数 */
	void (* delayus )( unsigned int UsDly );/* 微秒级别的延时函数 */

	//PHY的SMI接口读写函数
	unsigned int (* SMI_R)(unsigned char phy_add,unsigned char reg_add);   
	void (* SMI_W)(unsigned char phy_add,unsigned char reg_add,unsigned int wDATA);

	//加密芯片的IIC
	unsigned char (* IIC_WriteByte)(unsigned char device_addr, unsigned char sub_addr, unsigned char *buff, int ByteNo);
	unsigned char (* IIC_ReadByte)(unsigned char device_addr, unsigned char sub_addr, unsigned char *buff, int ByteNo);
	
	void (* ptz_printSTR )( char *str );
	void (* ptz_printCH )( unsigned char ch );
	  
}tdrusercfg;

void TdrInit(void);
//硬件提供接口
void TDR_Init(tdrusercfg *FunInterface);
int GetChipID(void);//芯片识别，可不用。返回值=0x1510时为88e1510；返回值=0x6320时为88e6320；返回值为0时，识别不到或出错

int TDRrun(void);
int TDRrun_P4(void);//88e6320 P4

int getresult(TDRresult *getDATA);
int getresult_P4(TDRresult *getDATA);//88e6320 P4

int GetSkew(char *skewValue);
int GetSkew_P4(char *skewValue);//88e6320 P4

void cable_tdr_test(void);
extern int appBuf[20];//普通测试数据
extern char skewBuf[4];//高级测试数据

#endif	




