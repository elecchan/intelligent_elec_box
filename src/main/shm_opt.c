#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>

#include "shmdata.h"
#include "shm_opt.h"

#define Debug 1

//分配的共享内存的原始首地址
void *shm_intelligent = NULL;
//共享内存标识符
int shmid_intelligent;

int creat_shm(void) 
{
	//1.创建2个共享内存
	shmid_intelligent = shmget(SHM_INTELLIGENT_ID, TEXT_SZ, 0666|IPC_CREAT);
	if(shmid_intelligent == -1)
	{
		printf("ipc shmget failed\n");
		exit(EXIT_FAILURE);
	}

	//2.将共享内存连接到当前进程的地址空间
	shm_intelligent = shmat(shmid_intelligent, 0, 0);
	if(shm_intelligent == (void*)-1)
	{
		printf("ipc shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("ipc Memory attached at %X\n", (int)shm_intelligent);
	
	//3.设置共享内存
	intelligent_conf = (struct shared_intelligent_conf*)shm_intelligent;

	return 0;
}

int delete_shm(void)
{
	//1.把共享内存从当前进程中分离
	if(shmdt(shm_intelligent) == -1)
	{
		printf("shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
