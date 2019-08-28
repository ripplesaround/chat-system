//读取user和密码
#include "chat.h"

#include<stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define MAXMSG 500			  //最大消息数
#define MAXUSER 500		  //用户最大数量
#define RW 0 				      //读写进程对共享内存区的信号量 初始值：1
#define MUTEX 1 			    //计数器信号量                初始值：1
#define W 2 				      //为了写进程优先设置的信号量   初始值：1
#define COUNT 3				    //读进程数量                  初始值：0
#define FILESEM 4 			  //文件访问信号量              初始值：1

int main()
{
	int fd;
	int num;
	int usernum;
	//char s[1000];
	User userinfo[MAXUSER];

	fd=open("userinfo.dat",O_RDONLY,0770);
	if(fd == -1){
		printf("file \"userinfo.dat\" opened failed!\n");
		return -1;
	}
	num=read(fd,&usernum,sizeof(int));
	read(fd,userinfo,MAXUSER*sizeof(User));
	//write(fd, s, 100);
	//User userinfo[MAXUSER];
	printf("%d\n",fd);
	printf("usernum = %d\n",usernum);
	for(int i=0;i<usernum;++i)
	{
		printf("%s\n",userinfo[i].account);
		printf("%s\n",userinfo[i].password);
		printf("%d\n\n",userinfo[i].user_id);
	}
	//printf("%s",s);
	return 0;
}
