//文件

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
	int num;
	int usernum;
	//int a;
	//char s[1000];
	//User userinfo[MAXUSER];
	Data data;
	//history/1to2.dat
	fd=open("history/3to1.dat",O_RDWR|O_CREAT,0660);
	if(fd == -1){
		printf("file \"nihaodat\" opened failed!\n");
		return -1;
	}
	num = read(fd,&usernum,sizeof(int));
	printf("%d\n",usernum);
	read(fd,&data,sizeof(data));
	// //write(fd, s, 100);
	// //User userinfo[MAXUSER];
	// printf("%d\n",fd);
	printf("data.str: %s\n",data.message.str);
	// for(int i=0;i<usernum;++i)
	// {
	// 	printf("%s\n",userinfo[i].account);
	// 	printf("%s\n\n",userinfo[i].password);
	// }
	return 0;
}