
#ifndef _TDR_H
#define _TDR_H
/*
����ʱ��
������ʾ---��˥��---�������İ�������ʾ

��·����·ʱ��
���Ȱ�������ʾ���߳�����10�ײ���˥����ֵ������ʾ---��������ʾ---
*/
typedef	struct _TDRresult		
{				//[0]~[4]��Ӧ�߶�1~4	
	unsigned char sta[4];	//�߶�״̬ 0:����; 1:��·; 2:��·��·��״̬Ϊ�����ߡ�ʱ�����Ȳ����ã���ʾ��---��	
	float length[4];		//�߶Գ��� ��λ:m	
	float atten[4];			//˥�� ��λ:dB/100m (�߳�����10�ײ���˥����ֵ) 
							// >-8.2dB-->good cable!   
							//>-11.8dB-->poor quality cable!
							//else-->wet cable!

	float ref[4];			//�����ʣ���λ:%����sta[]=0ʱ��Ч
	float imp[4];			//�迹����λ:ohm����sta[]=0ʱ��Ч
}TDRresult;
/*
lengthȡһλС��
attenȡһλС��
refȡһλС��
impȡ��������
ע��ȡС��ʱҪ��������
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
	void (* delayms )( unsigned int MsDly );/* ���뼶�����ʱ���� */
	void (* delayus )( unsigned int UsDly );/* ΢�뼶�����ʱ���� */

	//PHY��SMI�ӿڶ�д����
	unsigned int (* SMI_R)(unsigned char phy_add,unsigned char reg_add);   
	void (* SMI_W)(unsigned char phy_add,unsigned char reg_add,unsigned int wDATA);

	//����оƬ��IIC
	unsigned char (* IIC_WriteByte)(unsigned char device_addr, unsigned char sub_addr, unsigned char *buff, int ByteNo);
	unsigned char (* IIC_ReadByte)(unsigned char device_addr, unsigned char sub_addr, unsigned char *buff, int ByteNo);
	
	void (* ptz_printSTR )( char *str );
	void (* ptz_printCH )( unsigned char ch );
	  
}tdrusercfg;

void TdrInit(void);
//Ӳ���ṩ�ӿ�
void TDR_Init(tdrusercfg *FunInterface);
int GetChipID(void);//оƬʶ�𣬿ɲ��á�����ֵ=0x1510ʱΪ88e1510������ֵ=0x6320ʱΪ88e6320������ֵΪ0ʱ��ʶ�𲻵������

int TDRrun(void);
int TDRrun_P4(void);//88e6320 P4

int getresult(TDRresult *getDATA);
int getresult_P4(TDRresult *getDATA);//88e6320 P4

int GetSkew(char *skewValue);
int GetSkew_P4(char *skewValue);//88e6320 P4

void cable_tdr_test(void);
extern int appBuf[20];//��ͨ��������
extern char skewBuf[4];//�߼���������

#endif	




