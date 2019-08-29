//读取好友关系
#include "chat.h"

#include<stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define MAXMSG 500			  //最大消息数
#define MAXUSER 20		 	 //用户最大数量
#define RW 0 				      //读写进程对共享内存区的信号量 初始值：1
#define MUTEX 1 			    //计数器信号量                初始值：1
#define W 2 				      //为了写进程优先设置的信号量   初始值：1
#define COUNT 3				    //读进程数量                  初始值：0
#define FILESEM 4 			  //文件访问信号量              初始值：1

int main()
{
	int fd;

	int rel[MAXUSER+1][MAXUSER+1];
	fd=open("friend.dat",O_RDONLY,0770);
	if(fd == -1){
		printf("file \"friend.dat\" opened failed!\n");
		return -1;
	}
	read(fd, rel ,sizeof(rel));
	//write(fd, s, 100);
	//User userinfo[MAXUSER];
	printf("%d\n",fd);
	//printf("usernum = %d\n",usernum);
	for(int i=0;i<MAXUSER+1;++i)
	{
		for(int j=0;j<MAXUSER+1;++j)
		{
			printf("%d ",rel[i][j]);
		}
		printf("\n");
	}
	//printf("%s",s);
	return 0;
}
