#ifndef _SHMDATA_H_HEADER  
#define _SHMDATA_H_HEADER  
 
//共享内存ID
#define SHM_INTELLIGENT_ID    111
//申请共享内存大小，单位B
#define TEXT_SZ 4096  
 
//IPC配置信息
struct shared_intelligent_conf  
{  
	unsigned int useful;//数据是否有效
	unsigned int reserves[32];
};  
//全局配置信息结构体
struct shared_intelligent_conf *intelligent_conf;

#endif
